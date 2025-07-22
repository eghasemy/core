/*
  s_curve.h - true 7-phase S-curve acceleration implementation

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

#ifndef _S_CURVE_H_
#define _S_CURVE_H_

#include "hal.h"
#include "planner.h"

#if ENABLE_S_CURVE_ACCELERATION

// 7-phase S-curve motion profile phases
typedef enum {
    SCurvePhase_JerkUp = 0,         // Phase 1: Jerk-up (acceleration increasing)
    SCurvePhase_AccelConstant,      // Phase 2: Constant acceleration 
    SCurvePhase_JerkDown,           // Phase 3: Jerk-down (acceleration decreasing)
    SCurvePhase_Cruise,             // Phase 4: Constant velocity (cruise)
    SCurvePhase_DecelJerkUp,        // Phase 5: Deceleration jerk-up (negative acceleration increasing)
    SCurvePhase_DecelConstant,      // Phase 6: Constant deceleration
    SCurvePhase_DecelJerkDown,      // Phase 7: Deceleration jerk-down (deceleration decreasing)
    SCurvePhase_Complete            // Motion complete
} s_curve_phase_t;

// S-curve motion profile parameters
typedef struct {
    float jerk;                     // Maximum jerk (mm/sec³)
    float acceleration;             // Maximum acceleration (mm/sec²)
    float max_velocity;             // Maximum velocity (mm/sec)
    float distance;                 // Total distance to travel (mm)
    float initial_velocity;         // Initial velocity (mm/sec)
    float final_velocity;           // Final velocity (mm/sec)
    
    // Phase durations (calculated)
    float t_jerk_up;               // Duration of jerk-up phase
    float t_accel;                 // Duration of constant acceleration phase
    float t_jerk_down;             // Duration of jerk-down phase
    float t_cruise;                // Duration of cruise phase
    float t_decel_jerk_up;         // Duration of deceleration jerk-up phase
    float t_decel;                 // Duration of constant deceleration phase
    float t_decel_jerk_down;       // Duration of deceleration jerk-down phase
    
    // Phase distances (calculated)
    float d_jerk_up;               // Distance during jerk-up phase
    float d_accel;                 // Distance during constant acceleration phase
    float d_jerk_down;             // Distance during jerk-down phase
    float d_cruise;                // Distance during cruise phase
    float d_decel_jerk_up;         // Distance during deceleration jerk-up phase
    float d_decel;                 // Distance during constant deceleration phase
    float d_decel_jerk_down;       // Distance during deceleration jerk-down phase
    
    // Current state
    s_curve_phase_t current_phase; // Current motion phase
    float time_in_phase;           // Time elapsed in current phase
    bool is_valid;                 // True if profile is valid
} s_curve_profile_t;

// Runtime adjustable S-curve parameters
typedef struct {
    float jerk_xy;                 // XY axes jerk limit (mm/sec³)
    float jerk_z;                  // Z axis jerk limit (mm/sec³)
    float jerk_e;                  // E axis jerk limit (mm/sec³) - for extruder/rotary
    float jerk_multiplier;         // Global jerk multiplier (0.1 - 5.0)
    float corner_jerk_factor;      // Corner jerk reduction factor (0.1 - 1.0)
    float adaptive_jerk_enable;    // Enable adaptive jerk control
    float min_jerk_velocity;       // Minimum velocity for jerk limiting (mm/sec)
    bool enable_advanced_features; // Enable advanced S-curve features
} s_curve_settings_t;

// Function prototypes
bool s_curve_calculate_profile(s_curve_profile_t *profile, float distance, 
                              float initial_velocity, float final_velocity,
                              float max_velocity, float acceleration, float jerk);

float s_curve_get_velocity_at_time(const s_curve_profile_t *profile, float time);
float s_curve_get_acceleration_at_time(const s_curve_profile_t *profile, float time);
float s_curve_get_distance_at_time(const s_curve_profile_t *profile, float time);

s_curve_phase_t s_curve_get_phase_at_time(const s_curve_profile_t *profile, float time);

bool s_curve_update_settings(uint8_t axis, float jerk);
bool s_curve_set_jerk_multiplier(float multiplier);
bool s_curve_set_corner_factor(float factor);

void s_curve_init(void);
s_curve_settings_t* s_curve_get_settings(void);

// M-code interface initialization  
#if S_CURVE_ENABLE_MCODES
void s_curve_mcodes_init(void);
#endif

// Advanced features
bool s_curve_adaptive_jerk_calculate(const plan_block_t *block, float *adaptive_jerk);
float s_curve_junction_jerk_limit(float junction_angle, float nominal_jerk);

// Real-time parameter adjustment
typedef enum {
    SCurveParam_JerkXY = 0,
    SCurveParam_JerkZ,
    SCurveParam_JerkE,
    SCurveParam_JerkMultiplier,
    SCurveParam_CornerFactor,
    SCurveParam_AdaptiveEnable
} s_curve_param_t;

bool s_curve_set_parameter_realtime(s_curve_param_t param, float value);
float s_curve_get_parameter(s_curve_param_t param);

#endif // ENABLE_S_CURVE_ACCELERATION

#endif // _S_CURVE_H_