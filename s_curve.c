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

// External settings declaration
extern settings_t settings;

// Pre-calculated constants for STM32F446 FPU optimization
static const float one_sixth = 1.0f / 6.0f;     // 1/6 for cubic calculations
static const float one_half = 0.5f;             // 1/2 for quadratic calculations

// Initialize S-curve system
void s_curve_init(void)
{
    // S-curve settings are now managed by the global settings system
    // No local initialization needed
}

// Get current settings - return pointer to global settings structure
s_curve_settings_t* s_curve_get_settings(void)
{
    return &settings.s_curve;
}

// Calculate 7-phase S-curve motion profile
bool s_curve_calculate_profile(s_curve_profile_t *profile, float distance, 
                              float initial_velocity, float final_velocity,
                              float max_velocity, float acceleration, float jerk)
{
    return s_curve_calculate_profile_optimized(profile, distance, initial_velocity, 
                                             final_velocity, max_velocity, acceleration, 
                                             jerk, false);
}

// Calculate 7-phase S-curve motion profile with final deceleration optimization
bool s_curve_calculate_profile_optimized(s_curve_profile_t *profile, float distance, 
                                        float initial_velocity, float final_velocity,
                                        float max_velocity, float acceleration, float jerk,
                                        bool is_jog_motion)
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

    // Get S-curve settings for optimization
    s_curve_settings_t *s_settings = s_curve_get_settings();
    
    // Apply final deceleration optimization for jog motions or when ending at zero velocity
    bool optimize_final_decel = is_jog_motion || (final_velocity < 0.1f);
    float final_jerk_multiplier = 1.0f;
    float min_stop_velocity = 0.0f;
    float stop_threshold = 0.0f;
    
    if (optimize_final_decel && s_settings) {
        final_jerk_multiplier = s_settings->final_decel_jerk_multiplier;
        min_stop_velocity = s_settings->min_stop_velocity / 60.0f; // Convert mm/min to mm/sec
        stop_threshold = s_settings->stop_threshold_distance;
    }

    // Calculate phase durations for acceleration
    float t_jerk = acceleration / jerk;  // Time to reach max acceleration
    profile->t_jerk_up = t_jerk;
    profile->t_jerk_down = t_jerk;
    
    // Velocity at end of jerk-up phase
    float v_jerk_up = initial_velocity + one_half * jerk * t_jerk * t_jerk;
    
    // Velocity at start of jerk-down phase (target velocity)
    float v_target = max_velocity;
    
    // Calculate constant acceleration time
    float dv_accel = v_target - v_jerk_up - one_half * jerk * t_jerk * t_jerk;
    profile->t_accel = max(0.0f, dv_accel / acceleration);
    
    // Calculate deceleration phases with optimization
    if (optimize_final_decel && final_velocity < min_stop_velocity) {
        // Use optimized final deceleration
        float final_jerk = jerk * final_jerk_multiplier;
        float final_t_jerk = acceleration / final_jerk;
        
        // Calculate if we need to use threshold-based stopping
        float total_accel_distance = 0.0f;
        
        // Estimate acceleration distance
        total_accel_distance = initial_velocity * (profile->t_jerk_up + profile->t_accel + profile->t_jerk_down) +
                              0.5f * acceleration * (profile->t_accel * profile->t_accel + 
                              profile->t_jerk_up * profile->t_jerk_up + profile->t_jerk_down * profile->t_jerk_down);
        
        float remaining_distance = distance - total_accel_distance;
        
        if (remaining_distance > stop_threshold && stop_threshold > 0.0f) {
            // Use normal deceleration until threshold, then rapid stop
            float cruise_distance = remaining_distance - stop_threshold;
            profile->d_cruise = max(0.0f, cruise_distance);
            profile->t_cruise = profile->d_cruise / max_velocity;
            
            // Optimized final deceleration phases
            profile->t_decel_jerk_up = final_t_jerk * 0.5f;  // Shorter jerk-up for final phase
            profile->t_decel = 0.1f;  // Very short constant deceleration
            profile->t_decel_jerk_down = final_t_jerk * 0.3f;  // Much shorter final jerk-down
        } else {
            // Direct optimized deceleration
            profile->t_decel_jerk_up = final_t_jerk * 0.6f;
            profile->t_decel = profile->t_accel * 0.5f;  // Shorter deceleration phase
            profile->t_decel_jerk_down = final_t_jerk * 0.4f;  // Much shorter final phase
        }
    } else {
        // Standard symmetric deceleration
        profile->t_decel_jerk_up = t_jerk;
        profile->t_decel = profile->t_accel;
        profile->t_decel_jerk_down = t_jerk;
    }
    
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
            settings.axis[X_AXIS].jerk = jerk * 60.0f * 60.0f * 60.0f; // Convert to mm/min³
            settings.axis[Y_AXIS].jerk = jerk * 60.0f * 60.0f * 60.0f; // Convert to mm/min³
            break;
        case Z_AXIS:
            settings.axis[Z_AXIS].jerk = jerk * 60.0f * 60.0f * 60.0f; // Convert to mm/min³
            break;
        default:
            // For other axes (E axis etc.), we don't have a direct setting
            // This would need to be handled differently for systems with more axes
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
    
    settings.s_curve.multiplier = multiplier;
    return true;
}

// Set corner jerk reduction factor
bool s_curve_set_corner_factor(float factor)
{
    if (factor < 0.1f || factor > 1.0f) {
        return false;
    }
    
    settings.s_curve.corner_factor = factor;
    return true;
}

// Advanced adaptive jerk calculation
bool s_curve_adaptive_jerk_calculate(const plan_block_t *block, float *adaptive_jerk)
{
    if (!block || !adaptive_jerk || !true) {
        return false;
    }

    // Start with base jerk value
    float base_jerk = settings.axis[X_AXIS].jerk / (60.0f * 60.0f * 60.0f); // Default to XY
    
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
    *adaptive_jerk *= settings.s_curve.multiplier;
    
    return true;
}

// Calculate junction jerk limit based on angle
float s_curve_junction_jerk_limit(float junction_angle, float nominal_jerk)
{
    if (!true) {
        return nominal_jerk;
    }

    // Reduce jerk for sharp corners
    float angle_factor = 1.0f;
    if (junction_angle < M_PI_2) { // Less than 90 degrees
        angle_factor = settings.s_curve.corner_factor;
    }
    
    return nominal_jerk * angle_factor * settings.s_curve.multiplier;
}

// Real-time parameter adjustment - updates global settings
bool s_curve_set_parameter_realtime(s_curve_param_t param, float value)
{
    switch (param) {
        case SCurveParam_JerkXY:
            // Note: Individual axis jerk is stored in settings.axis[].jerk
            // This is a compatibility function - jerk should be set per axis
            if (value >= 0.0f && value <= 100000.0f) {
                // Update X and Y axis jerk settings
                settings.axis[X_AXIS].jerk = value * 60.0f * 60.0f * 60.0f; // Convert to mm/min³
                settings.axis[Y_AXIS].jerk = value * 60.0f * 60.0f * 60.0f; // Convert to mm/min³
                return true;
            }
            return false; // Explicitly return false if validation fails
            
        case SCurveParam_JerkZ:
            if (value >= 0.0f && value <= 100000.0f) {
                settings.axis[Z_AXIS].jerk = value * 60.0f * 60.0f * 60.0f; // Convert to mm/min³
                return true;
            }
            return false; // Explicitly return false if validation fails
            
        case SCurveParam_JerkE:
            // E axis handling for rotary/extruder
            if (value >= 0.0f && value <= 100000.0f) {
#ifdef A_AXIS
                settings.axis[A_AXIS].jerk = value * 60.0f * 60.0f * 60.0f; // Convert to mm/min³
#endif
                return true;
            }
            return false; // Explicitly return false if validation fails
            
        case SCurveParam_JerkMultiplier:
            if (value >= 0.1f && value <= 5.0f) {
                settings.s_curve.multiplier = value;
                return true;
            }
            return false; // Explicitly return false if validation fails
            
        case SCurveParam_CornerFactor:
            if (value >= 0.1f && value <= 1.0f) {
                settings.s_curve.corner_factor = value;
                return true;
            }
            return false; // Explicitly return false if validation fails
            
        case SCurveParam_AdaptiveEnable:
            settings.s_curve.adaptive_enable = (value > 0.0f);
            return true;
            
        case SCurveParam_JunctionVelocityFactor:
            if (value >= 0.5f && value <= 2.0f) {
                settings.s_curve.junction_velocity_factor = value;
                return true;
            }
            return false; // Explicitly return false if validation fails
            
        case SCurveParam_JunctionJerkMultiplier:
            if (value >= 0.1f && value <= 2.0f) {
                settings.s_curve.junction_jerk_multiplier = value;
                return true;
            }
            return false; // Explicitly return false if validation fails
            
        case SCurveParam_SmoothJunctionAngle:
            if (value >= 0.0f && value <= M_PI) {
                settings.s_curve.junction_angle_threshold = value * (180.0f / M_PI); // Convert radians to degrees
                return true;
            }
            return false; // Explicitly return false if validation fails
            
        case SCurveParam_EnablePathBlending:
            settings.s_curve.path_blending_enable = (value > 0.0f);
            return true;
            
        case SCurveParam_BlendTolerance:
            if (value >= 0.001f && value <= 1.0f) {
                settings.s_curve.path_blending_tolerance = value;
                return true;
            }
            return false; // Explicitly return false if validation fails
            
        case SCurveParam_MaxBlendRadius:
            if (value >= 0.1f && value <= 10.0f) {
                settings.s_curve.path_blending_radius = value;
                return true;
            }
            return false; // Explicitly return false if validation fails
            
        case SCurveParam_MinBlendVelocity:
            if (value >= 1.0f && value <= 100.0f) {
                settings.s_curve.path_blending_min_velocity = value * 60.0f; // Convert mm/sec to mm/min
                return true;
            }
            return false; // Explicitly return false if validation fails
            
        case SCurveParam_BlendJerkFactor:
            if (value >= 0.1f && value <= 1.0f) {
                settings.s_curve.path_blending_jerk_factor = value;
                return true;
            }
            return false; // Explicitly return false if validation fails
            
        case SCurveParam_LookaheadBlocks:
            if (value >= 3 && value <= 16) {
                settings.s_curve.path_blending_lookahead = (uint8_t)value;
                return true;
            }
            return false; // Explicitly return false if validation fails
            
        case SCurveParam_MinStopVelocity:
            if (value >= 0.1f && value <= 1000.0f) {
                settings.s_curve.min_stop_velocity = value;
                return true;
            }
            return false; // Explicitly return false if validation fails
            
        case SCurveParam_FinalDecelJerkMultiplier:
            if (value >= 0.1f && value <= 5.0f) {
                settings.s_curve.final_decel_jerk_multiplier = value;
                return true;
            }
            return false; // Explicitly return false if validation fails
            
        case SCurveParam_StopThresholdDistance:
            if (value >= 0.0f && value <= 50.0f) {
                settings.s_curve.stop_threshold_distance = value;
                return true;
            }
            return false; // Explicitly return false if validation fails
    }
    
    return false;
}

// Get parameter value from global settings
float s_curve_get_parameter(s_curve_param_t param)
{
    switch (param) {
        case SCurveParam_JerkXY:
            // Return X-axis jerk converted from mm/min³ to mm/sec³
            return settings.axis[X_AXIS].jerk / (60.0f * 60.0f * 60.0f);
        case SCurveParam_JerkZ:
            return settings.axis[Z_AXIS].jerk / (60.0f * 60.0f * 60.0f);
        case SCurveParam_JerkE:
#ifdef A_AXIS
            return settings.axis[A_AXIS].jerk / (60.0f * 60.0f * 60.0f);
#else
            return 120.0f; // Default value
#endif
        case SCurveParam_JerkMultiplier:
            return settings.s_curve.multiplier;
        case SCurveParam_CornerFactor:
            return settings.s_curve.corner_factor;
        case SCurveParam_AdaptiveEnable:
            return settings.s_curve.adaptive_enable ? 1.0f : 0.0f;
        case SCurveParam_JunctionVelocityFactor:
            return settings.s_curve.junction_velocity_factor;
        case SCurveParam_JunctionJerkMultiplier:
            return settings.s_curve.junction_jerk_multiplier;
        case SCurveParam_SmoothJunctionAngle:
            return settings.s_curve.junction_angle_threshold * (M_PI / 180.0f); // Convert degrees to radians
        case SCurveParam_EnablePathBlending:
            return settings.s_curve.path_blending_enable ? 1.0f : 0.0f;
        case SCurveParam_BlendTolerance:
            return settings.s_curve.path_blending_tolerance;
        case SCurveParam_MaxBlendRadius:
            return settings.s_curve.path_blending_radius;
        case SCurveParam_MinBlendVelocity:
            return settings.s_curve.path_blending_min_velocity / 60.0f; // Convert mm/min to mm/sec
        case SCurveParam_BlendJerkFactor:
            return settings.s_curve.path_blending_jerk_factor;
        case SCurveParam_LookaheadBlocks:
            return (float)settings.s_curve.path_blending_lookahead;
        case SCurveParam_MinStopVelocity:
            return settings.s_curve.min_stop_velocity;
        case SCurveParam_FinalDecelJerkMultiplier:
            return settings.s_curve.final_decel_jerk_multiplier;
        case SCurveParam_StopThresholdDistance:
            return settings.s_curve.stop_threshold_distance;
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
    if (!true) {
        return min(current_velocity, next_velocity);
    }
    
    // Base velocity limit from geometry
    float velocity_limit = min(current_velocity, next_velocity) * settings.s_curve.junction_velocity_factor;
    
    // Apply jerk-based velocity limit for S-curve
    float junction_jerk = jerk_limit * settings.s_curve.junction_jerk_multiplier;
    
    // For sharp corners, reduce velocity more aggressively
    if (junction_angle < (settings.s_curve.junction_angle_threshold * M_PI / 180.0f)) {
        float angle_factor = junction_angle / (settings.s_curve.junction_angle_threshold * M_PI / 180.0f);
        velocity_limit *= (0.5f + 0.5f * angle_factor); // Scale from 50% to 100%
    }
    
    // S-curve specific jerk limit consideration
    // For smooth S-curve transitions, we need to ensure jerk limits are respected
    float jerk_velocity_limit = sqrtf(junction_jerk * settings.s_curve.path_blending_tolerance);
    velocity_limit = min(velocity_limit, jerk_velocity_limit);
    
    return max(velocity_limit, 5.0f);
}

// Optimize junction velocity for S-curve motion
bool s_curve_optimize_junction_velocity(s_curve_junction_t *junction, 
                                       const plan_block_t *current_block,
                                       const plan_block_t *next_block)
{
    if (!junction || !current_block || !next_block || !true) {
        return false;
    }
    
    // Calculate junction velocity limit with S-curve awareness
    float base_jerk = settings.axis[X_AXIS].jerk / (60.0f * 60.0f * 60.0f); // Default to XY jerk
    
    // Apply adaptive jerk if enabled
    if (settings.s_curve.adaptive_enable > 0.0f) {
        // Reduce jerk for short moves
        if (current_block->millimeters < 1.0f || next_block->millimeters < 1.0f) {
            base_jerk *= 0.7f;
        }
        // Increase jerk for long straight moves
        else if (current_block->millimeters > 5.0f && next_block->millimeters > 5.0f && 
                 junction->junction_angle > (settings.s_curve.junction_angle_threshold * M_PI / 180.0f)) {
            base_jerk *= 1.3f;
        }
    }
    
    junction->jerk_limit = base_jerk * settings.s_curve.multiplier;
    
    // Calculate optimized junction velocity
    float current_velocity = sqrtf(current_block->entry_speed_sqr);
    float next_velocity = sqrtf(next_block->max_entry_speed_sqr);
    
    junction->optimal_junction_velocity = s_curve_calculate_junction_velocity_limit(
        junction->junction_angle, current_velocity, next_velocity, junction->jerk_limit);
    
    // Enable blending if conditions are met
    if (settings.s_curve.path_blending_enable && 
        junction->optimal_junction_velocity >= (settings.s_curve.path_blending_min_velocity / 60.0f) &&
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
    if (!junction || !true) {
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
    if (!current || !next || !blend_radius || !settings.s_curve.path_blending_enable) {
        return false;
    }
    
    // Calculate blend radius based on velocity, jerk limits and geometry
    float velocity = min(sqrtf(current->entry_speed_sqr), sqrtf(next->max_entry_speed_sqr));
    float jerk_limit = settings.axis[X_AXIS].jerk / (60.0f * 60.0f * 60.0f) * settings.s_curve.path_blending_jerk_factor;
    
    // Geometric constraint on blend radius
    float min_segment_length = min(current->millimeters, next->millimeters);
    float max_radius_geometric = min_segment_length * 0.25f; // Max 25% of shortest segment
    
    // Jerk-based constraint on blend radius
    float max_radius_jerk = velocity * velocity / jerk_limit;
    
    // Angle-based constraint
    float sin_half_angle = sinf(junction_angle * 0.5f);
    float max_radius_angle = settings.s_curve.path_blending_tolerance / sin_half_angle;
    
    // Take minimum of all constraints
    *blend_radius = min(min(max_radius_geometric, max_radius_jerk), 
                       min(max_radius_angle, settings.s_curve.path_blending_radius));
    
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
            
            if (junction->junction_angle < (settings.s_curve.junction_angle_threshold * M_PI / 180.0f)) {
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
    if (settings.s_curve.path_blending_enable) {
        s_curve_blend_settings_t blend_settings = {
            .enabled = true,
            .tolerance = settings.s_curve.path_blending_tolerance,
            .max_blend_radius = settings.s_curve.path_blending_radius,
            .min_blend_velocity = (settings.s_curve.path_blending_min_velocity / 60.0f),
            .blend_jerk_factor = settings.s_curve.path_blending_jerk_factor,
            .lookahead_blocks = settings.s_curve.path_blending_lookahead
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
    
    blend_settings.enabled = settings.s_curve.path_blending_enable;
    blend_settings.tolerance = settings.s_curve.path_blending_tolerance;
    blend_settings.max_blend_radius = settings.s_curve.path_blending_radius;
    blend_settings.min_blend_velocity = (settings.s_curve.path_blending_min_velocity / 60.0f);
    blend_settings.blend_jerk_factor = settings.s_curve.path_blending_jerk_factor;
    blend_settings.lookahead_blocks = settings.s_curve.path_blending_lookahead;
    
    return &blend_settings;
}

// Set blend tolerance
bool s_curve_set_blend_tolerance(float tolerance)
{
    if (tolerance >= 0.001f && tolerance <= 1.0f) {
        settings.s_curve.path_blending_tolerance = tolerance;
        return true;
    }
    return false;
}

// Set blend jerk factor  
bool s_curve_set_blend_jerk_factor(float factor)
{
    if (factor >= 0.1f && factor <= 1.0f) {
        settings.s_curve.path_blending_jerk_factor = factor;
        return true;
    }
    return false;
}

#endif // ENABLE_S_CURVE_ACCELERATION