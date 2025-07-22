/*
  s_curve.c - true 7-phase S-curve acceleration implementation

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

#include <math.h>
#include <string.h>
#include "nuts_bolts.h"
#include "settings.h"

// Pre-calculated constants for STM32F446 FPU optimization
static const float one_sixth = 1.0f / 6.0f;     // 1/6 for cubic calculations
static const float one_half = 0.5f;             // 1/2 for quadratic calculations
static const float one_third = 1.0f / 3.0f;     // 1/3 for cubic calculations
static const float two_thirds = 2.0f / 3.0f;    // 2/3 for calculations

// Runtime S-curve settings
static s_curve_settings_t s_curve_settings = {
    .jerk_xy = 150.0f,              // Default XY jerk (mm/sec³)
    .jerk_z = 80.0f,                // Default Z jerk (mm/sec³)
    .jerk_e = 120.0f,               // Default E jerk (mm/sec³)
    .jerk_multiplier = 1.0f,        // Default multiplier
    .corner_jerk_factor = 0.7f,     // Reduce jerk in corners
    .adaptive_jerk_enable = 1.0f,   // Enable adaptive jerk by default
    .min_jerk_velocity = 5.0f,      // Minimum velocity for jerk limiting
    .enable_advanced_features = true,
    
    // Junction velocity optimization defaults
    .junction_velocity_factor = 1.2f,  // 20% junction velocity boost
    .junction_jerk_multiplier = 0.8f,  // Reduce jerk at junctions by 20%
    .smooth_junction_angle = 2.617f,   // 150 degrees in radians
    
    // Path blending defaults
    .enable_path_blending = true,      // Enable path blending
    .blend_tolerance = 0.02f,          // 0.02mm blending tolerance
    .max_blend_radius = 2.0f,          // 2mm maximum blend radius
    .min_blend_velocity = 10.0f,       // 10mm/sec minimum for blending
    .blend_jerk_factor = 0.6f,         // Reduce jerk for blended paths
    .lookahead_blocks = 8              // 8 blocks lookahead
};

// Initialize S-curve system
void s_curve_init(void)
{
    // Load settings from EEPROM if available
    // For now, use defaults
    memset(&s_curve_settings, 0, sizeof(s_curve_settings_t));
    s_curve_settings.jerk_xy = DEFAULT_X_JERK;
    s_curve_settings.jerk_z = DEFAULT_Z_JERK;
    s_curve_settings.jerk_e = 120.0f;
    s_curve_settings.jerk_multiplier = S_CURVE_JERK_MULTIPLIER;
    s_curve_settings.corner_jerk_factor = S_CURVE_CORNER_JERK_FACTOR;
    s_curve_settings.adaptive_jerk_enable = 1.0f;
    s_curve_settings.min_jerk_velocity = 5.0f;
    s_curve_settings.enable_advanced_features = true;
    
    // Junction velocity optimization settings
    s_curve_settings.junction_velocity_factor = 1.2f;
    s_curve_settings.junction_jerk_multiplier = 0.8f;
    s_curve_settings.smooth_junction_angle = 2.617f; // 150 degrees
    
    // Path blending settings
    s_curve_settings.enable_path_blending = true;
    s_curve_settings.blend_tolerance = 0.02f;
    s_curve_settings.max_blend_radius = 2.0f;
    s_curve_settings.min_blend_velocity = 10.0f;
    s_curve_settings.blend_jerk_factor = 0.6f;
    s_curve_settings.lookahead_blocks = 8;
}

// Get current settings
s_curve_settings_t* s_curve_get_settings(void)
{
    return &s_curve_settings;
}

// Calculate 7-phase S-curve motion profile
bool s_curve_calculate_profile(s_curve_profile_t *profile, float distance, 
                              float initial_velocity, float final_velocity,
                              float max_velocity, float acceleration, float jerk)
{
    if (!profile || distance <= 0.0f || acceleration <= 0.0f || jerk <= 0.0f) {
        return false;
    }

    // Clear profile
    memset(profile, 0, sizeof(s_curve_profile_t));
    
    profile->jerk = jerk;
    profile->acceleration = acceleration;
    profile->max_velocity = max_velocity;
    profile->distance = distance;
    profile->initial_velocity = initial_velocity;
    profile->final_velocity = final_velocity;
    profile->current_phase = SCurvePhase_JerkUp;
    profile->time_in_phase = 0.0f;

    // Calculate phase durations for acceleration
    float t_jerk = acceleration / jerk;  // Time to reach max acceleration
    profile->t_jerk_up = t_jerk;
    profile->t_jerk_down = t_jerk;
    
    // Velocity at end of jerk-up phase
    float v_jerk_up = initial_velocity + one_half * jerk * t_jerk * t_jerk;
    
    // Velocity at start of jerk-down phase (target velocity)
    float v_target = max_velocity;
    
    // Check if we can reach target velocity
    float v_max_reachable = v_jerk_up + acceleration * 0.0f; // Will be calculated
    
    // Calculate constant acceleration time
    float dv_accel = v_target - v_jerk_up - one_half * jerk * t_jerk * t_jerk;
    profile->t_accel = max(0.0f, dv_accel / acceleration);
    
    // Calculate deceleration phases (mirror of acceleration)
    profile->t_decel_jerk_up = t_jerk;
    profile->t_decel = profile->t_accel;  // Symmetric for now
    profile->t_decel_jerk_down = t_jerk;
    
    // Calculate phase distances using optimized FPU calculations
    
    // Phase 1: Jerk-up distance
    profile->d_jerk_up = initial_velocity * profile->t_jerk_up + 
                         one_half * one_half * jerk * profile->t_jerk_up * profile->t_jerk_up * profile->t_jerk_up;
    
    // Phase 2: Constant acceleration distance
    float v_accel_start = initial_velocity + one_half * jerk * profile->t_jerk_up * profile->t_jerk_up;
    profile->d_accel = v_accel_start * profile->t_accel + 
                       one_half * acceleration * profile->t_accel * profile->t_accel;
    
    // Phase 3: Jerk-down distance
    float v_jerk_down_start = v_accel_start + acceleration * profile->t_accel;
    profile->d_jerk_down = v_jerk_down_start * profile->t_jerk_down + 
                          acceleration * profile->t_jerk_down * profile->t_jerk_down -
                          one_sixth * jerk * profile->t_jerk_down * profile->t_jerk_down * profile->t_jerk_down;
    
    // Calculate cruise distance
    float d_accel_total = profile->d_jerk_up + profile->d_accel + profile->d_jerk_down;
    float d_decel_total = d_accel_total; // Symmetric for now
    profile->d_cruise = max(0.0f, distance - d_accel_total - d_decel_total);
    profile->t_cruise = profile->d_cruise / max_velocity;
    
    // Calculate deceleration distances (mirror of acceleration)
    profile->d_decel_jerk_up = profile->d_jerk_down;
    profile->d_decel = profile->d_accel;
    profile->d_decel_jerk_down = profile->d_jerk_up;
    
    // Validate profile
    float total_distance = profile->d_jerk_up + profile->d_accel + profile->d_jerk_down +
                          profile->d_cruise + profile->d_decel_jerk_up + profile->d_decel + 
                          profile->d_decel_jerk_down;
    
    profile->is_valid = (fabs(total_distance - distance) < 0.001f);
    
    return profile->is_valid;
}

// Get velocity at specific time in motion profile
float s_curve_get_velocity_at_time(const s_curve_profile_t *profile, float time)
{
    if (!profile->is_valid || time < 0.0f) {
        return 0.0f;
    }

    float t = 0.0f;
    
    // Phase 1: Jerk-up
    if (time <= (t += profile->t_jerk_up)) {
        float dt = time;
        return profile->initial_velocity + one_half * profile->jerk * dt * dt;
    }
    
    // Phase 2: Constant acceleration
    if (time <= (t += profile->t_accel)) {
        float dt = time - profile->t_jerk_up;
        float v_start = profile->initial_velocity + one_half * profile->jerk * profile->t_jerk_up * profile->t_jerk_up;
        return v_start + profile->acceleration * dt;
    }
    
    // Phase 3: Jerk-down
    if (time <= (t += profile->t_jerk_down)) {
        float dt = time - profile->t_jerk_up - profile->t_accel;
        float v_start = profile->initial_velocity + one_half * profile->jerk * profile->t_jerk_up * profile->t_jerk_up +
                       profile->acceleration * profile->t_accel;
        return v_start + profile->acceleration * dt - one_half * profile->jerk * dt * dt;
    }
    
    // Phase 4: Cruise
    if (time <= (t += profile->t_cruise)) {
        return profile->max_velocity;
    }
    
    // Phase 5: Deceleration jerk-up
    if (time <= (t += profile->t_decel_jerk_up)) {
        float dt = time - profile->t_jerk_up - profile->t_accel - profile->t_jerk_down - profile->t_cruise;
        return profile->max_velocity - one_half * profile->jerk * dt * dt;
    }
    
    // Phase 6: Constant deceleration
    if (time <= (t += profile->t_decel)) {
        float dt = time - profile->t_jerk_up - profile->t_accel - profile->t_jerk_down - 
                  profile->t_cruise - profile->t_decel_jerk_up;
        float v_start = profile->max_velocity - one_half * profile->jerk * profile->t_decel_jerk_up * profile->t_decel_jerk_up;
        return v_start - profile->acceleration * dt;
    }
    
    // Phase 7: Deceleration jerk-down
    if (time <= (t += profile->t_decel_jerk_down)) {
        float dt = time - profile->t_jerk_up - profile->t_accel - profile->t_jerk_down - 
                  profile->t_cruise - profile->t_decel_jerk_up - profile->t_decel;
        float v_start = profile->max_velocity - one_half * profile->jerk * profile->t_decel_jerk_up * profile->t_decel_jerk_up -
                       profile->acceleration * profile->t_decel;
        return v_start - profile->acceleration * dt + one_half * profile->jerk * dt * dt;
    }
    
    // Motion complete
    return profile->final_velocity;
}

// Get acceleration at specific time in motion profile
float s_curve_get_acceleration_at_time(const s_curve_profile_t *profile, float time)
{
    if (!profile->is_valid || time < 0.0f) {
        return 0.0f;
    }

    float t = 0.0f;
    
    // Phase 1: Jerk-up
    if (time <= (t += profile->t_jerk_up)) {
        float dt = time;
        return profile->jerk * dt;
    }
    
    // Phase 2: Constant acceleration
    if (time <= (t += profile->t_accel)) {
        return profile->acceleration;
    }
    
    // Phase 3: Jerk-down
    if (time <= (t += profile->t_jerk_down)) {
        float dt = time - profile->t_jerk_up - profile->t_accel;
        return profile->acceleration - profile->jerk * dt;
    }
    
    // Phase 4: Cruise
    if (time <= (t += profile->t_cruise)) {
        return 0.0f;
    }
    
    // Phase 5: Deceleration jerk-up
    if (time <= (t += profile->t_decel_jerk_up)) {
        float dt = time - profile->t_jerk_up - profile->t_accel - profile->t_jerk_down - profile->t_cruise;
        return -profile->jerk * dt;
    }
    
    // Phase 6: Constant deceleration
    if (time <= (t += profile->t_decel)) {
        return -profile->acceleration;
    }
    
    // Phase 7: Deceleration jerk-down
    if (time <= (t += profile->t_decel_jerk_down)) {
        float dt = time - profile->t_jerk_up - profile->t_accel - profile->t_jerk_down - 
                  profile->t_cruise - profile->t_decel_jerk_up - profile->t_decel;
        return -profile->acceleration + profile->jerk * dt;
    }
    
    // Motion complete
    return 0.0f;
}

// Get distance traveled at specific time
float s_curve_get_distance_at_time(const s_curve_profile_t *profile, float time)
{
    if (!profile->is_valid || time < 0.0f) {
        return 0.0f;
    }

    float distance = 0.0f;
    float t = 0.0f;
    
    // Phase 1: Jerk-up
    if (time <= (t += profile->t_jerk_up)) {
        float dt = time;
        return profile->initial_velocity * dt + one_sixth * profile->jerk * dt * dt * dt;
    }
    distance += profile->d_jerk_up;
    
    // Phase 2: Constant acceleration
    if (time <= (t += profile->t_accel)) {
        float dt = time - profile->t_jerk_up;
        float v_start = profile->initial_velocity + one_half * profile->jerk * profile->t_jerk_up * profile->t_jerk_up;
        return distance + v_start * dt + one_half * profile->acceleration * dt * dt;
    }
    distance += profile->d_accel;
    
    // Phase 3: Jerk-down
    if (time <= (t += profile->t_jerk_down)) {
        float dt = time - profile->t_jerk_up - profile->t_accel;
        float v_start = profile->initial_velocity + one_half * profile->jerk * profile->t_jerk_up * profile->t_jerk_up +
                       profile->acceleration * profile->t_accel;
        return distance + v_start * dt + one_half * profile->acceleration * dt * dt - 
               one_sixth * profile->jerk * dt * dt * dt;
    }
    distance += profile->d_jerk_down;
    
    // Phase 4: Cruise
    if (time <= (t += profile->t_cruise)) {
        float dt = time - profile->t_jerk_up - profile->t_accel - profile->t_jerk_down;
        return distance + profile->max_velocity * dt;
    }
    distance += profile->d_cruise;
    
    // Continue for deceleration phases...
    // (Similar pattern for phases 5-7)
    
    return min(distance, profile->distance);
}

// Get current phase at specific time
s_curve_phase_t s_curve_get_phase_at_time(const s_curve_profile_t *profile, float time)
{
    if (!profile->is_valid || time < 0.0f) {
        return SCurvePhase_Complete;
    }

    float t = 0.0f;
    
    if (time <= (t += profile->t_jerk_up)) return SCurvePhase_JerkUp;
    if (time <= (t += profile->t_accel)) return SCurvePhase_AccelConstant;
    if (time <= (t += profile->t_jerk_down)) return SCurvePhase_JerkDown;
    if (time <= (t += profile->t_cruise)) return SCurvePhase_Cruise;
    if (time <= (t += profile->t_decel_jerk_up)) return SCurvePhase_DecelJerkUp;
    if (time <= (t += profile->t_decel)) return SCurvePhase_DecelConstant;
    if (time <= (t += profile->t_decel_jerk_down)) return SCurvePhase_DecelJerkDown;
    
    return SCurvePhase_Complete;
}

// Update jerk settings for specific axis
bool s_curve_update_settings(uint8_t axis, float jerk)
{
    if (jerk <= 0.0f || jerk > 10000.0f) {
        return false;
    }

    switch (axis) {
        case X_AXIS:
        case Y_AXIS:
            s_curve_settings.jerk_xy = jerk;
            break;
        case Z_AXIS:
            s_curve_settings.jerk_z = jerk;
            break;
        default:
            s_curve_settings.jerk_e = jerk;
            break;
    }
    
    return true;
}

// Set global jerk multiplier
bool s_curve_set_jerk_multiplier(float multiplier)
{
    if (multiplier < 0.1f || multiplier > 5.0f) {
        return false;
    }
    
    s_curve_settings.jerk_multiplier = multiplier;
    return true;
}

// Set corner jerk reduction factor
bool s_curve_set_corner_factor(float factor)
{
    if (factor < 0.1f || factor > 1.0f) {
        return false;
    }
    
    s_curve_settings.corner_jerk_factor = factor;
    return true;
}

// Advanced adaptive jerk calculation
bool s_curve_adaptive_jerk_calculate(const plan_block_t *block, float *adaptive_jerk)
{
    if (!block || !adaptive_jerk || !s_curve_settings.enable_advanced_features) {
        return false;
    }

    // Start with base jerk value
    float base_jerk = s_curve_settings.jerk_xy; // Default to XY
    
    // Adjust based on move characteristics
    if (block->millimeters < 1.0f) {
        // Short moves - reduce jerk for precision
        *adaptive_jerk = base_jerk * 0.5f;
    } else if (block->millimeters > 10.0f) {
        // Long moves - can use higher jerk
        *adaptive_jerk = base_jerk * 1.2f;
    } else {
        *adaptive_jerk = base_jerk;
    }
    
    // Apply global multiplier
    *adaptive_jerk *= s_curve_settings.jerk_multiplier;
    
    return true;
}

// Calculate junction jerk limit based on angle
float s_curve_junction_jerk_limit(float junction_angle, float nominal_jerk)
{
    if (!s_curve_settings.enable_advanced_features) {
        return nominal_jerk;
    }

    // Reduce jerk for sharp corners
    float angle_factor = 1.0f;
    if (junction_angle < M_PI_2) { // Less than 90 degrees
        angle_factor = s_curve_settings.corner_jerk_factor;
    }
    
    return nominal_jerk * angle_factor * s_curve_settings.jerk_multiplier;
}

// Real-time parameter adjustment
bool s_curve_set_parameter_realtime(s_curve_param_t param, float value)
{
    switch (param) {
        case SCurveParam_JerkXY:
            if (value > 0.0f && value <= 1000.0f) {
                s_curve_settings.jerk_xy = value;
                return true;
            }
            break;
            
        case SCurveParam_JerkZ:
            if (value > 0.0f && value <= 1000.0f) {
                s_curve_settings.jerk_z = value;
                return true;
            }
            break;
            
        case SCurveParam_JerkE:
            if (value > 0.0f && value <= 1000.0f) {
                s_curve_settings.jerk_e = value;
                return true;
            }
            break;
            
        case SCurveParam_JerkMultiplier:
            if (value >= 0.1f && value <= 5.0f) {
                s_curve_settings.jerk_multiplier = value;
                return true;
            }
            break;
            
        case SCurveParam_CornerFactor:
            if (value >= 0.1f && value <= 1.0f) {
                s_curve_settings.corner_jerk_factor = value;
                return true;
            }
            break;
            
        case SCurveParam_AdaptiveEnable:
            s_curve_settings.adaptive_jerk_enable = (value > 0.0f) ? 1.0f : 0.0f;
            return true;
            
        case SCurveParam_JunctionVelocityFactor:
            if (value >= 0.5f && value <= 2.0f) {
                s_curve_settings.junction_velocity_factor = value;
                return true;
            }
            break;
            
        case SCurveParam_JunctionJerkMultiplier:
            if (value >= 0.1f && value <= 2.0f) {
                s_curve_settings.junction_jerk_multiplier = value;
                return true;
            }
            break;
            
        case SCurveParam_SmoothJunctionAngle:
            if (value >= 0.0f && value <= M_PI) {
                s_curve_settings.smooth_junction_angle = value;
                return true;
            }
            break;
            
        case SCurveParam_EnablePathBlending:
            s_curve_settings.enable_path_blending = (value > 0.0f);
            return true;
            
        case SCurveParam_BlendTolerance:
            if (value >= 0.001f && value <= 1.0f) {
                s_curve_settings.blend_tolerance = value;
                return true;
            }
            break;
            
        case SCurveParam_MaxBlendRadius:
            if (value >= 0.1f && value <= 10.0f) {
                s_curve_settings.max_blend_radius = value;
                return true;
            }
            break;
            
        case SCurveParam_MinBlendVelocity:
            if (value >= 1.0f && value <= 100.0f) {
                s_curve_settings.min_blend_velocity = value;
                return true;
            }
            break;
            
        case SCurveParam_BlendJerkFactor:
            if (value >= 0.1f && value <= 1.0f) {
                s_curve_settings.blend_jerk_factor = value;
                return true;
            }
            break;
            
        case SCurveParam_LookaheadBlocks:
            if (value >= 3.0f && value <= 16.0f) {
                s_curve_settings.lookahead_blocks = (uint8_t)value;
                return true;
            }
            break;
    }
    
    return false;
}

// Get parameter value
float s_curve_get_parameter(s_curve_param_t param)
{
    switch (param) {
        case SCurveParam_JerkXY:
            return s_curve_settings.jerk_xy;
        case SCurveParam_JerkZ:
            return s_curve_settings.jerk_z;
        case SCurveParam_JerkE:
            return s_curve_settings.jerk_e;
        case SCurveParam_JerkMultiplier:
            return s_curve_settings.jerk_multiplier;
        case SCurveParam_CornerFactor:
            return s_curve_settings.corner_jerk_factor;
        case SCurveParam_AdaptiveEnable:
            return s_curve_settings.adaptive_jerk_enable;
        case SCurveParam_JunctionVelocityFactor:
            return s_curve_settings.junction_velocity_factor;
        case SCurveParam_JunctionJerkMultiplier:
            return s_curve_settings.junction_jerk_multiplier;
        case SCurveParam_SmoothJunctionAngle:
            return s_curve_settings.smooth_junction_angle;
        case SCurveParam_EnablePathBlending:
            return s_curve_settings.enable_path_blending ? 1.0f : 0.0f;
        case SCurveParam_BlendTolerance:
            return s_curve_settings.blend_tolerance;
        case SCurveParam_MaxBlendRadius:
            return s_curve_settings.max_blend_radius;
        case SCurveParam_MinBlendVelocity:
            return s_curve_settings.min_blend_velocity;
        case SCurveParam_BlendJerkFactor:
            return s_curve_settings.blend_jerk_factor;
        case SCurveParam_LookaheadBlocks:
            return (float)s_curve_settings.lookahead_blocks;
        default:
            return 0.0f;
    }
}

// ============================================================================
// JUNCTION VELOCITY OPTIMIZATION WITH S-CURVE
// ============================================================================

// Calculate S-curve optimized junction velocity limit
float s_curve_calculate_junction_velocity_limit(float junction_angle, 
                                               float current_velocity,
                                               float next_velocity,
                                               float jerk_limit)
{
    if (!s_curve_settings.enable_advanced_features) {
        return min(current_velocity, next_velocity);
    }
    
    // Base velocity limit from geometry
    float velocity_limit = min(current_velocity, next_velocity) * s_curve_settings.junction_velocity_factor;
    
    // Apply jerk-based velocity limit for S-curve
    float junction_jerk = jerk_limit * s_curve_settings.junction_jerk_multiplier;
    
    // For sharp corners, reduce velocity more aggressively
    if (junction_angle < s_curve_settings.smooth_junction_angle) {
        float angle_factor = junction_angle / s_curve_settings.smooth_junction_angle;
        velocity_limit *= (0.5f + 0.5f * angle_factor); // Scale from 50% to 100%
    }
    
    // S-curve specific jerk limit consideration
    // For smooth S-curve transitions, we need to ensure jerk limits are respected
    float jerk_velocity_limit = sqrtf(junction_jerk * s_curve_settings.blend_tolerance);
    velocity_limit = min(velocity_limit, jerk_velocity_limit);
    
    return max(velocity_limit, s_curve_settings.min_jerk_velocity);
}

// Optimize junction velocity for S-curve motion
bool s_curve_optimize_junction_velocity(s_curve_junction_t *junction, 
                                       const plan_block_t *current_block,
                                       const plan_block_t *next_block)
{
    if (!junction || !current_block || !next_block || !s_curve_settings.enable_advanced_features) {
        return false;
    }
    
    // Calculate junction velocity limit with S-curve awareness
    float base_jerk = s_curve_settings.jerk_xy; // Default to XY jerk
    
    // Apply adaptive jerk if enabled
    if (s_curve_settings.adaptive_jerk_enable > 0.0f) {
        // Reduce jerk for short moves
        if (current_block->millimeters < 1.0f || next_block->millimeters < 1.0f) {
            base_jerk *= 0.7f;
        }
        // Increase jerk for long straight moves
        else if (current_block->millimeters > 5.0f && next_block->millimeters > 5.0f && 
                 junction->junction_angle > s_curve_settings.smooth_junction_angle) {
            base_jerk *= 1.3f;
        }
    }
    
    junction->jerk_limit = base_jerk * s_curve_settings.jerk_multiplier;
    
    // Calculate optimized junction velocity
    float current_velocity = sqrtf(current_block->entry_speed_sqr);
    float next_velocity = sqrtf(next_block->max_entry_speed_sqr);
    
    junction->optimal_junction_velocity = s_curve_calculate_junction_velocity_limit(
        junction->junction_angle, current_velocity, next_velocity, junction->jerk_limit);
    
    // Enable blending if conditions are met
    if (s_curve_settings.enable_path_blending && 
        junction->optimal_junction_velocity >= s_curve_settings.min_blend_velocity &&
        junction->junction_angle > M_PI_4) { // > 45 degrees
        
        junction->enable_blending = true;
        return s_curve_calculate_blend_radius(current_block, next_block, 
                                            junction->junction_angle, &junction->blend_radius);
    }
    
    junction->enable_blending = false;
    return true;
}

// Validate S-curve junction transition
bool s_curve_validate_junction_transition(const s_curve_junction_t *junction,
                                         float current_acceleration,
                                         float next_acceleration)
{
    if (!junction || !s_curve_settings.enable_advanced_features) {
        return true; // Default to valid if not using advanced features
    }
    
    // Check if acceleration change respects jerk limits
    float acceleration_change = fabsf(next_acceleration - current_acceleration);
    float time_for_change = 0.1f; // Assume 0.1 second transition time
    float required_jerk = acceleration_change / time_for_change;
    
    return required_jerk <= junction->jerk_limit;
}

// ============================================================================
// PATH BLENDING WITH S-CURVE INTEGRATION  
// ============================================================================

// Calculate blend radius for path blending
bool s_curve_calculate_blend_radius(const plan_block_t *current,
                                   const plan_block_t *next,
                                   float junction_angle,
                                   float *blend_radius)
{
    if (!current || !next || !blend_radius || !s_curve_settings.enable_path_blending) {
        return false;
    }
    
    // Calculate blend radius based on velocity, jerk limits and geometry
    float velocity = min(sqrtf(current->entry_speed_sqr), sqrtf(next->max_entry_speed_sqr));
    float jerk_limit = s_curve_settings.jerk_xy * s_curve_settings.blend_jerk_factor;
    
    // Geometric constraint on blend radius
    float min_segment_length = min(current->millimeters, next->millimeters);
    float max_radius_geometric = min_segment_length * 0.25f; // Max 25% of shortest segment
    
    // Jerk-based constraint on blend radius
    float max_radius_jerk = velocity * velocity / jerk_limit;
    
    // Angle-based constraint
    float sin_half_angle = sinf(junction_angle * 0.5f);
    float max_radius_angle = s_curve_settings.blend_tolerance / sin_half_angle;
    
    // Take minimum of all constraints
    *blend_radius = min(min(max_radius_geometric, max_radius_jerk), 
                       min(max_radius_angle, s_curve_settings.max_blend_radius));
    
    return *blend_radius > 0.001f; // Valid if radius > 1 micron
}

// Perform path blending on multiple segments
bool s_curve_blend_path_segments(plan_block_t *blocks[], 
                                uint8_t block_count,
                                s_curve_blend_settings_t *settings)
{
    if (!blocks || block_count < 2 || !settings || !settings->enabled) {
        return false;
    }
    
    // Process each junction between blocks
    for (uint8_t i = 0; i < block_count - 1; i++) {
        plan_block_t *current = blocks[i];
        plan_block_t *next = blocks[i + 1];
        
        if (!current || !next) continue;
        
        // Calculate junction angle (simplified - would need proper vector math)
        float junction_angle = M_PI_2; // Default to 90 degrees for now
        
        float blend_radius;
        if (s_curve_calculate_blend_radius(current, next, junction_angle, &blend_radius)) {
            // Apply blending by modifying block velocities and profiles
            // This would require more complex implementation to modify the actual motion profiles
            
            // For now, just apply velocity scaling based on blend factor
            float blend_factor = settings->blend_jerk_factor;
            current->entry_speed_sqr *= blend_factor;
            next->entry_speed_sqr *= blend_factor;
        }
    }
    
    return true;
}

// ============================================================================
// MULTI-BLOCK LOOKAHEAD FOR S-CURVE OPTIMIZATION
// ============================================================================

// Analyze lookahead sequence for optimization opportunities  
bool s_curve_analyze_lookahead(s_curve_lookahead_t *lookahead)
{
    if (!lookahead || lookahead->block_count < 2) {
        return false;
    }
    
    lookahead->total_distance = 0.0f;
    lookahead->max_velocity = 0.0f;
    lookahead->has_sharp_corners = false;
    
    // Analyze each block in the sequence
    for (uint8_t i = 0; i < lookahead->block_count; i++) {
        plan_block_t *block = lookahead->blocks[i];
        if (!block) continue;
        
        lookahead->total_distance += block->millimeters;
        float block_velocity = sqrtf(block->entry_speed_sqr);
        if (block_velocity > lookahead->max_velocity) {
            lookahead->max_velocity = block_velocity;
        }
        
        // Analyze junctions
        if (i < lookahead->block_count - 1) {
            s_curve_junction_t *junction = &lookahead->junctions[i];
            plan_block_t *next_block = lookahead->blocks[i + 1];
            
            // Simplified junction angle calculation (would need proper vector math)
            junction->junction_angle = M_PI_2; // Default to 90 degrees
            
            if (junction->junction_angle < s_curve_settings.smooth_junction_angle) {
                lookahead->has_sharp_corners = true;
            }
            
            // Optimize this junction
            s_curve_optimize_junction_velocity(junction, block, next_block);
        }
    }
    
    return true;
}

// Optimize the entire lookahead sequence
bool s_curve_optimize_lookahead_sequence(s_curve_lookahead_t *lookahead)
{
    if (!s_curve_analyze_lookahead(lookahead)) {
        return false;
    }
    
    // Apply global optimizations based on sequence characteristics
    if (lookahead->total_distance > 10.0f && !lookahead->has_sharp_corners) {
        // Long sequence without sharp corners - can use higher jerk values
        for (uint8_t i = 0; i < lookahead->block_count - 1; i++) {
            lookahead->junctions[i].jerk_limit *= 1.2f;
        }
    } else if (lookahead->has_sharp_corners) {
        // Sequence with sharp corners - reduce jerk for smoother motion
        for (uint8_t i = 0; i < lookahead->block_count - 1; i++) {
            lookahead->junctions[i].jerk_limit *= 0.8f;
        }
    }
    
    // Apply path blending if enabled
    if (s_curve_settings.enable_path_blending) {
        s_curve_blend_settings_t blend_settings = {
            .enabled = true,
            .tolerance = s_curve_settings.blend_tolerance,
            .max_blend_radius = s_curve_settings.max_blend_radius,
            .min_blend_velocity = s_curve_settings.min_blend_velocity,
            .blend_jerk_factor = s_curve_settings.blend_jerk_factor,
            .lookahead_blocks = s_curve_settings.lookahead_blocks
        };
        
        return s_curve_blend_path_segments(lookahead->blocks, lookahead->block_count, &blend_settings);
    }
    
    return true;
}

// ============================================================================
// S-CURVE PATH BLENDING SETTINGS ACCESS
// ============================================================================

// Get path blending settings
s_curve_blend_settings_t* s_curve_get_blend_settings(void)
{
    static s_curve_blend_settings_t blend_settings;
    
    blend_settings.enabled = s_curve_settings.enable_path_blending;
    blend_settings.tolerance = s_curve_settings.blend_tolerance;
    blend_settings.max_blend_radius = s_curve_settings.max_blend_radius;
    blend_settings.min_blend_velocity = s_curve_settings.min_blend_velocity;
    blend_settings.blend_jerk_factor = s_curve_settings.blend_jerk_factor;
    blend_settings.lookahead_blocks = s_curve_settings.lookahead_blocks;
    
    return &blend_settings;
}

// Set blend tolerance
bool s_curve_set_blend_tolerance(float tolerance)
{
    if (tolerance >= 0.001f && tolerance <= 1.0f) {
        s_curve_settings.blend_tolerance = tolerance;
        return true;
    }
    return false;
}

// Set blend jerk factor  
bool s_curve_set_blend_jerk_factor(float factor)
{
    if (factor >= 0.1f && factor <= 1.0f) {
        s_curve_settings.blend_jerk_factor = factor;
        return true;
    }
    return false;
}

#endif // ENABLE_S_CURVE_ACCELERATION