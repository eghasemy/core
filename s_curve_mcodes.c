/*
  s_curve_mcodes.c - M-code handlers for S-curve parameter adjustment

  Part of grblHAL

  Copyright (c) 2025 grblHAL contributors

  grblHAL is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  grblHAL is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with grblHAL. If not, see <http://www.gnu.org/licenses/>.
*/

#include "s_curve.h"

#if ENABLE_S_CURVE_ACCELERATION

#include "gcode.h"
#include "protocol.h"
#include "report.h"
#include <stdio.h>
#include <math.h>

// From gcode.c - coordinate array indices for arc offsets
#define I_VALUE X_AXIS
#define J_VALUE Y_AXIS  
#define K_VALUE Z_AXIS

// M-code definitions for S-curve control
typedef enum {
    MCode_SetAcceleration = 204,        // M204 - Set acceleration (P=print/travel, R=retract, T=travel)
    MCode_SetJerk = 205,                // M205 - Set jerk (X=xy, Z=z, E=extruder, J=junction_deviation)
    MCode_SCurveAdvanced = 206,         // M206 - S-curve advanced parameters
    MCode_SCurveReport = 207,           // M207 - Report S-curve parameters
    MCode_SCurveReset = 208,            // M208 - Reset S-curve parameters to defaults
    MCode_SCurveProfile = 209,          // M209 - Set S-curve profile (adaptive, corner factor)
    MCode_JunctionOptimize = 210,       // M210 - Junction velocity optimization settings
    MCode_PathBlending = 211            // M211 - Path blending configuration
} s_curve_mcode_t;

// Forward declarations
static user_mcode_type_t s_curve_mcode_check(user_mcode_t mcode);
static status_code_t s_curve_mcode_validate(parser_block_t *gc_block);
static void s_curve_mcode_execute(sys_state_t state, parser_block_t *gc_block);

// User M-code handler structure
static user_mcode_ptrs_t s_curve_mcodes = {
    .check = s_curve_mcode_check,
    .validate = s_curve_mcode_validate,
    .execute = s_curve_mcode_execute
};

// Initialize S-curve M-code handlers
void s_curve_mcodes_init(void)
{
    // Register the M-code handlers with grblHAL
    grbl.user_mcode = s_curve_mcodes;
}

// Check if M-code is handled by S-curve system
static user_mcode_type_t s_curve_mcode_check(user_mcode_t mcode)
{
    switch (mcode) {
        case MCode_SetAcceleration:
        case MCode_SetJerk:
        case MCode_SCurveAdvanced:
        case MCode_SCurveReport:
        case MCode_SCurveReset:
        case MCode_SCurveProfile:
        case MCode_JunctionOptimize:
        case MCode_PathBlending:
            return UserMCode_Normal;
        default:
            return UserMCode_Unsupported;
    }
}

// Validate M-code parameters
static status_code_t s_curve_mcode_validate(parser_block_t *gc_block)
{
    status_code_t state = Status_OK;
    
    switch (gc_block->user_mcode) {
        case MCode_SetAcceleration:
            // M204: P<print_accel> R<retract_accel> T<travel_accel>
            if (isnan(gc_block->values.p) && isnan(gc_block->values.r) && gc_block->values.t == 0) {
                state = Status_GcodeValueWordMissing;
            }
            break;
            
        case MCode_SetJerk:
            // M205: X<xy_jerk> Z<z_jerk> E<e_jerk> J<junction_deviation>
            if (isnan(gc_block->values.xyz[X_AXIS]) && isnan(gc_block->values.xyz[Z_AXIS]) && 
                isnan(gc_block->values.e) && isnan(gc_block->values.ijk[J_VALUE])) {
                state = Status_GcodeValueWordMissing;
            }
            break;
            
        case MCode_SCurveAdvanced:
            // M206: M<multiplier> C<corner_factor> A<adaptive_enable>
            if (isnan(gc_block->values.m) && isnan(gc_block->values.c) && 
#ifndef A_AXIS  
                isnan(gc_block->values.a)) {
#else
                gc_block->values.xyz[A_AXIS] == 0.0f) {
#endif
                state = Status_GcodeValueWordMissing;
            }
            break;
            
        case MCode_SCurveReport:
        case MCode_SCurveReset:
            // No parameters required
            break;
            
        case MCode_SCurveProfile:
            // M209: S<profile_type> V<value>
            if (isnan(gc_block->values.s)) {
                state = Status_GcodeValueWordMissing;
            }
            break;
            
        case MCode_JunctionOptimize:
            // M210: Junction velocity optimization
            // F<velocity_factor> J<jerk_multiplier> A<angle_threshold>
            if (isnan(gc_block->values.f) && isnan(gc_block->values.ijk[J_VALUE]) && 
#ifndef A_AXIS  
                isnan(gc_block->values.a)) {
#else
                gc_block->values.xyz[A_AXIS] == 0.0f) {
#endif
                state = Status_GcodeValueWordMissing;
            }
            break;
            
        case MCode_PathBlending:
            // M211: Path blending configuration  
            // S<enable> P<tolerance> R<max_radius> V<min_velocity> F<jerk_factor> L<lookahead_blocks>
            if (isnan(gc_block->values.s)) {
                state = Status_GcodeValueWordMissing;
            }
            break;
            
        default:
            state = Status_GcodeUnsupportedCommand;
            break;
    }
    
    return state;
}

// Execute M-code
static void s_curve_mcode_execute(sys_state_t state, parser_block_t *gc_block)
{
    bool ok = true;
    char msg[100]; // Increased buffer size for longer messages
    
    switch (gc_block->user_mcode) {
        case MCode_SetAcceleration:
            // M204: Set acceleration
            if (!isnan(gc_block->values.p)) {
                // P parameter: print/travel acceleration for XY
                for (uint8_t idx = 0; idx < 2; idx++) { // X and Y axes
                    ok &= settings_override_acceleration(idx, gc_block->values.p);
                }
            }
            if (!isnan(gc_block->values.r)) {
                // R parameter: retract acceleration (typically for E axis)
                if (N_AXIS > 3) {
                    ok &= settings_override_acceleration(3, gc_block->values.r);
                }
            }
            if (gc_block->values.t != 0) {
                // T parameter: travel acceleration for Z
                ok &= settings_override_acceleration(Z_AXIS, (float)gc_block->values.t);
            }
            if (ok) {
                hal.stream.write("[MSG:Acceleration updated]" ASCII_EOL);
            } else {
                hal.stream.write("[MSG:ERR: Invalid acceleration values]" ASCII_EOL);
            }
            break;
            
        case MCode_SetJerk:
            // M205: Set jerk parameters
            if (!isnan(gc_block->values.xyz[X_AXIS])) {
                ok &= s_curve_set_parameter_realtime(SCurveParam_JerkXY, gc_block->values.xyz[X_AXIS]);
            }
            if (!isnan(gc_block->values.xyz[Z_AXIS])) {
                ok &= s_curve_set_parameter_realtime(SCurveParam_JerkZ, gc_block->values.xyz[Z_AXIS]);
            }
            if (!isnan(gc_block->values.e)) {
                ok &= s_curve_set_parameter_realtime(SCurveParam_JerkE, gc_block->values.e);
            }
            if (!isnan(gc_block->values.ijk[J_VALUE])) {
                // Junction deviation - update corner factor
                float corner_factor = max(0.1f, min(1.0f, 1.0f - gc_block->values.ijk[J_VALUE] * 10.0f));
                ok &= s_curve_set_parameter_realtime(SCurveParam_CornerFactor, corner_factor);
            }
            if (ok) {
                hal.stream.write("[MSG:Jerk parameters updated]" ASCII_EOL);
            } else {
                hal.stream.write("[MSG:ERR: Invalid jerk values]" ASCII_EOL);
            }
            break;
            
        case MCode_SCurveAdvanced:
            // M206: Advanced S-curve parameters
            if (!isnan(gc_block->values.m)) {
                ok &= s_curve_set_parameter_realtime(SCurveParam_JerkMultiplier, gc_block->values.m);
            }
#ifndef C_AXIS
            if (!isnan(gc_block->values.c)) {
                ok &= s_curve_set_parameter_realtime(SCurveParam_CornerFactor, gc_block->values.c);
            }
#else
            if (!isnan(gc_block->values.xyz[C_AXIS])) {
                ok &= s_curve_set_parameter_realtime(SCurveParam_CornerFactor, gc_block->values.xyz[C_AXIS]);
            }
#endif
#ifndef A_AXIS
            if (!isnan(gc_block->values.a)) {
                ok &= s_curve_set_parameter_realtime(SCurveParam_AdaptiveEnable, gc_block->values.a);
            }
#else
            if (gc_block->values.xyz[A_AXIS] != 0.0f) {
                ok &= s_curve_set_parameter_realtime(SCurveParam_AdaptiveEnable, gc_block->values.xyz[A_AXIS]);
            }
#endif
            if (ok) {
                hal.stream.write("[MSG:S-curve advanced parameters updated]" ASCII_EOL);
            } else {
                hal.stream.write("[MSG:ERR: Invalid S-curve parameters]" ASCII_EOL);
            }
            break;
            
        case MCode_SCurveReport:
            // M207: Report current S-curve parameters
            {
                s_curve_settings_t *settings = s_curve_get_settings();
                hal.stream.write("[MSG:S-Curve Parameters:]" ASCII_EOL);
                sprintf(msg, "[MSG:XY Jerk: %.1f mm/sec^3]" ASCII_EOL, settings->jerk_xy);
                hal.stream.write(msg);
                sprintf(msg, "[MSG:Z Jerk: %.1f mm/sec^3]" ASCII_EOL, settings->jerk_z);
                hal.stream.write(msg);
                sprintf(msg, "[MSG:E Jerk: %.1f mm/sec^3]" ASCII_EOL, settings->jerk_e);
                hal.stream.write(msg);
                sprintf(msg, "[MSG:Multiplier: %.2f]" ASCII_EOL, settings->jerk_multiplier);
                hal.stream.write(msg);
                sprintf(msg, "[MSG:Corner Factor: %.2f]" ASCII_EOL, settings->corner_jerk_factor);
                hal.stream.write(msg);
                sprintf(msg, "[MSG:Adaptive: %s]" ASCII_EOL, 
                       settings->adaptive_jerk_enable > 0.0f ? "ON" : "OFF");
                hal.stream.write(msg);
                
                // Junction optimization parameters
                hal.stream.write("[MSG:Junction Optimization:]" ASCII_EOL);
                sprintf(msg, "[MSG:Velocity Factor: %.2f]" ASCII_EOL, settings->junction_velocity_factor);
                hal.stream.write(msg);
                sprintf(msg, "[MSG:Jerk Multiplier: %.2f]" ASCII_EOL, settings->junction_jerk_multiplier);
                hal.stream.write(msg);
                sprintf(msg, "[MSG:Smooth Angle: %.1f deg]" ASCII_EOL, settings->smooth_junction_angle * 180.0f / M_PI);
                hal.stream.write(msg);
                
                // Path blending parameters
                hal.stream.write("[MSG:Path Blending:]" ASCII_EOL);
                sprintf(msg, "[MSG:Enabled: %s]" ASCII_EOL, settings->enable_path_blending ? "ON" : "OFF");
                hal.stream.write(msg);
                sprintf(msg, "[MSG:Tolerance: %.3f mm]" ASCII_EOL, settings->blend_tolerance);
                hal.stream.write(msg);
                sprintf(msg, "[MSG:Max Radius: %.2f mm]" ASCII_EOL, settings->max_blend_radius);
                hal.stream.write(msg);
                sprintf(msg, "[MSG:Min Velocity: %.1f mm/sec]" ASCII_EOL, settings->min_blend_velocity);
                hal.stream.write(msg);
                sprintf(msg, "[MSG:Jerk Factor: %.2f]" ASCII_EOL, settings->blend_jerk_factor);
                hal.stream.write(msg);
                sprintf(msg, "[MSG:Lookahead Blocks: %d]" ASCII_EOL, settings->lookahead_blocks);
                hal.stream.write(msg);
            }
            break;
            
        case MCode_SCurveReset:
            // M208: Reset S-curve parameters to defaults
            s_curve_init(); // Reinitialize with defaults
            hal.stream.write("[MSG:S-curve parameters reset to defaults]" ASCII_EOL);
            break;
            
        case MCode_SCurveProfile:
            // M209: Set S-curve profile options
            if (!isnan(gc_block->values.s)) {
                int profile_type = (int)gc_block->values.s;
#ifndef V_AXIS
                float value = isnan(gc_block->values.v) ? 1.0f : gc_block->values.v;
#else
                float value = (gc_block->values.xyz[V_AXIS] == 0.0f) ? 1.0f : gc_block->values.xyz[V_AXIS];
#endif
                
                switch (profile_type) {
                    case 0: // Disable adaptive jerk
                        ok = s_curve_set_parameter_realtime(SCurveParam_AdaptiveEnable, 0.0f);
                        break;
                    case 1: // Enable adaptive jerk
                        ok = s_curve_set_parameter_realtime(SCurveParam_AdaptiveEnable, 1.0f);
                        break;
                    case 2: // Set corner factor
                        ok = s_curve_set_parameter_realtime(SCurveParam_CornerFactor, value);
                        break;
                    case 3: // Set global multiplier
                        ok = s_curve_set_parameter_realtime(SCurveParam_JerkMultiplier, value);
                        break;
                    default:
                        ok = false;
                        break;
                }
                
                if (ok) {
                    sprintf(msg, "[MSG:S-curve profile %d set to %.2f]" ASCII_EOL, profile_type, value);
                    hal.stream.write(msg);
                } else {
                    hal.stream.write("[MSG:ERR: Invalid profile type or value]" ASCII_EOL);
                }
            }
            break;
            
        case MCode_JunctionOptimize:
            // M210: Junction velocity optimization settings
            if (!isnan(gc_block->values.f)) {
                ok &= s_curve_set_parameter_realtime(SCurveParam_JunctionVelocityFactor, gc_block->values.f);
            }
            if (!isnan(gc_block->values.ijk[J_VALUE])) {
                ok &= s_curve_set_parameter_realtime(SCurveParam_JunctionJerkMultiplier, gc_block->values.ijk[J_VALUE]);
            }
#ifndef A_AXIS
            if (!isnan(gc_block->values.a)) {
                ok &= s_curve_set_parameter_realtime(SCurveParam_SmoothJunctionAngle, gc_block->values.a * M_PI / 180.0f);
            }
#else
            if (gc_block->values.xyz[A_AXIS] != 0.0f) {
                ok &= s_curve_set_parameter_realtime(SCurveParam_SmoothJunctionAngle, gc_block->values.xyz[A_AXIS] * M_PI / 180.0f);
            }
#endif
            if (ok) {
                hal.stream.write("[MSG:Junction optimization parameters updated]" ASCII_EOL);
                s_curve_settings_t *settings = s_curve_get_settings();
                sprintf(msg, "[MSG:Velocity Factor: %.2f, Jerk Factor: %.2f]" ASCII_EOL, 
                       settings->junction_velocity_factor, settings->junction_jerk_multiplier);
                hal.stream.write(msg);
            } else {
                hal.stream.write("[MSG:ERR: Invalid junction optimization values]" ASCII_EOL);
            }
            break;
            
        case MCode_PathBlending:
            // M211: Path blending configuration
            if (!isnan(gc_block->values.s)) {
                bool enable = (gc_block->values.s > 0.0f);
                ok &= s_curve_set_parameter_realtime(SCurveParam_EnablePathBlending, enable ? 1.0f : 0.0f);
            }
            if (!isnan(gc_block->values.p)) {
                // P parameter for tolerance (replacing T)
                ok &= s_curve_set_parameter_realtime(SCurveParam_BlendTolerance, gc_block->values.p);
            }
            if (!isnan(gc_block->values.r)) {
                ok &= s_curve_set_parameter_realtime(SCurveParam_MaxBlendRadius, gc_block->values.r);
            }
#ifndef V_AXIS
            if (!isnan(gc_block->values.v)) {
                ok &= s_curve_set_parameter_realtime(SCurveParam_MinBlendVelocity, gc_block->values.v);
            }
#else
            if (gc_block->values.xyz[V_AXIS] != 0.0f) {
                ok &= s_curve_set_parameter_realtime(SCurveParam_MinBlendVelocity, gc_block->values.xyz[V_AXIS]);
            }
#endif
            if (!isnan(gc_block->values.f)) {
                ok &= s_curve_set_parameter_realtime(SCurveParam_BlendJerkFactor, gc_block->values.f);
            }
            if (gc_block->values.l != 0) {
                // L parameter is uint8_t
                ok &= s_curve_set_parameter_realtime(SCurveParam_LookaheadBlocks, (float)gc_block->values.l);
            }
            if (ok) {
                hal.stream.write("[MSG:Path blending configuration updated]" ASCII_EOL);
                s_curve_settings_t *settings = s_curve_get_settings();
                sprintf(msg, "[MSG:Blending: %s, Tolerance: %.3f mm]" ASCII_EOL, 
                       settings->enable_path_blending ? "ON" : "OFF", settings->blend_tolerance);
                hal.stream.write(msg);
                sprintf(msg, "[MSG:Max Radius: %.2f mm, Min Velocity: %.1f mm/sec]" ASCII_EOL, 
                       settings->max_blend_radius, settings->min_blend_velocity);
                hal.stream.write(msg);
            } else {
                hal.stream.write("[MSG:ERR: Invalid path blending values]" ASCII_EOL);
            }
            break;
            
        default:
            break;
    }
}

#endif // ENABLE_S_CURVE_ACCELERATION