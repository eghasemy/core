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
    
    // Junction velocity optimization parameters
    float junction_velocity_factor; // Junction velocity optimization factor (0.5 - 2.0)
    float junction_jerk_multiplier; // Junction-specific jerk multiplier (0.1 - 2.0)
    float smooth_junction_angle;    // Angle threshold for smooth junctions (radians)
    
    // Path blending parameters
    bool enable_path_blending;     // Enable S-curve path blending
    float blend_tolerance;         // Path blending tolerance (mm) 
    float max_blend_radius;        // Maximum blend radius (mm)
    float min_blend_velocity;      // Minimum velocity for blending (mm/sec)
    float blend_jerk_factor;       // Jerk factor for blended paths (0.1-1.0)
    uint8_t lookahead_blocks;      // Number of blocks for lookahead (3-16)
} s_curve_runtime_settings_t;

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
s_curve_runtime_settings_t* s_curve_get_settings(void);

// M-code interface initialization  
#if S_CURVE_ENABLE_MCODES
void s_curve_mcodes_init(void);
#endif

// Advanced features
bool s_curve_adaptive_jerk_calculate(const plan_block_t *block, float *adaptive_jerk);
float s_curve_junction_jerk_limit(float junction_angle, float nominal_jerk);

// Junction Velocity Optimization with S-Curve
typedef struct {
    float junction_angle;           // Junction angle in radians
    float entry_velocity;          // Entry velocity (mm/sec)
    float exit_velocity;           // Exit velocity (mm/sec)
    float max_junction_velocity;   // Maximum allowable junction velocity
    float optimal_junction_velocity; // S-curve optimized junction velocity
    float jerk_limit;              // Jerk limit for this junction
    float blend_radius;            // Path blending radius if applicable
    bool enable_blending;          // Enable path blending for this junction
    s_curve_profile_t entry_profile; // S-curve profile for entry
    s_curve_profile_t exit_profile;  // S-curve profile for exit
} s_curve_junction_t;

// Junction velocity optimization functions
bool s_curve_optimize_junction_velocity(s_curve_junction_t *junction, 
                                       const plan_block_t *current_block,
                                       const plan_block_t *next_block);

float s_curve_calculate_junction_velocity_limit(float junction_angle, 
                                               float current_velocity,
                                               float next_velocity,
                                               float jerk_limit);

bool s_curve_validate_junction_transition(const s_curve_junction_t *junction,
                                         float current_acceleration,
                                         float next_acceleration);

// Path blending with S-curve integration
typedef struct {
    bool enabled;                   // Path blending enabled
    float tolerance;               // Blending tolerance (mm)
    float max_blend_radius;        // Maximum blend radius (mm) 
    float min_blend_velocity;      // Minimum velocity for blending (mm/sec)
    float blend_jerk_factor;       // Jerk factor for blended paths (0.1-1.0)
    uint8_t lookahead_blocks;      // Number of blocks for lookahead
} s_curve_blend_settings_t;

bool s_curve_calculate_blend_radius(const plan_block_t *current,
                                   const plan_block_t *next,
                                   float junction_angle,
                                   float *blend_radius);

bool s_curve_blend_path_segments(plan_block_t *blocks[], 
                                uint8_t block_count,
                                s_curve_blend_settings_t *settings);

// Multi-block lookahead for S-curve optimization
typedef struct {
    plan_block_t *blocks[16];      // Up to 16 blocks lookahead
    uint8_t block_count;           // Number of blocks in lookahead
    float total_distance;          // Total distance of lookahead
    float max_velocity;            // Maximum velocity in sequence
    bool has_sharp_corners;        // Contains sharp corners requiring jerk limiting
    s_curve_junction_t junctions[15]; // Junctions between blocks (n-1 junctions)
} s_curve_lookahead_t;

bool s_curve_analyze_lookahead(s_curve_lookahead_t *lookahead);
bool s_curve_optimize_lookahead_sequence(s_curve_lookahead_t *lookahead);

// S-curve path blending settings access
s_curve_blend_settings_t* s_curve_get_blend_settings(void);
bool s_curve_set_blend_tolerance(float tolerance);
bool s_curve_set_blend_jerk_factor(float factor);

// Real-time parameter adjustment
typedef enum {
    SCurveParam_JerkXY = 0,
    SCurveParam_JerkZ,
    SCurveParam_JerkE,
    SCurveParam_JerkMultiplier,
    SCurveParam_CornerFactor,
    SCurveParam_AdaptiveEnable,
    SCurveParam_JunctionVelocityFactor,
    SCurveParam_JunctionJerkMultiplier,
    SCurveParam_SmoothJunctionAngle,
    SCurveParam_EnablePathBlending,
    SCurveParam_BlendTolerance,
    SCurveParam_MaxBlendRadius,
    SCurveParam_MinBlendVelocity,
    SCurveParam_BlendJerkFactor,
    SCurveParam_LookaheadBlocks
} s_curve_param_t;

bool s_curve_set_parameter_realtime(s_curve_param_t param, float value);
float s_curve_get_parameter(s_curve_param_t param);

#endif // ENABLE_S_CURVE_ACCELERATION

#endif // _S_CURVE_H_