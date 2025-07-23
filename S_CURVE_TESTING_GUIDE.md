# S-Curve Testing and Tuning Guide for CNC Machines

This guide provides rigorous testing procedures and G-code files to validate and optimize S-curve acceleration performance on higher-end CNC machines. Designed for NEMA23/NEMA24 stepper motors and servo systems (JMC 180W DC, 400W Stepperonline A6 series).

## Overview

### Testing Methodology

The testing process follows a systematic approach:
1. **Baseline Tests** - Verify S-curve functionality and establish baseline performance
2. **Parameter Sweep Tests** - Test all available S-curve parameters systematically  
3. **Material-Specific Tests** - Validate performance across different machining materials
4. **Tool-Specific Tests** - Optimize for different cutting tools and operations
5. **Stress Tests** - Push limits to find optimal performance boundaries
6. **Production Validation** - Real-world verification with actual parts

### Test Categories

**Functional Tests:** Verify basic S-curve operation
**Performance Tests:** Measure quantitative improvements  
**Accuracy Tests:** Validate dimensional precision
**Surface Quality Tests:** Assess finish improvements
**Reliability Tests:** Ensure consistent performance
**Edge Case Tests:** Validate behavior in challenging scenarios

## Prerequisites

### Machine Requirements
- [ ] CNC machine with NEMA23/24 steppers OR servo motors
- [ ] **Travel limits: 500x500x200mm minimum** (X/Y/Z axes)
- [ ] grblHAL controller with S-curve acceleration enabled
- [ ] Proper machine calibration ($100-$102 steps/mm accurate)
- [ ] Mechanical condition verified (no binding, proper lubrication)
- [ ] Tool holding system properly calibrated
- [ ] Work coordinate system established with sufficient clearance (25mm margins)

### Measurement Tools
- Digital calipers (0.02mm resolution minimum)
- Surface roughness gauge (recommended)
- Dial indicators for runout measurement
- Precision square for corner accuracy
- Timer or stopwatch for cycle time measurement
- Digital camera for motion recording (optional)

### Safety Equipment
- Safety glasses and hearing protection
- Emergency stop accessible
- Proper work holding and clamping
- First aid kit available

### Test Materials
- Aluminum 6061 blocks (100x100x25mm minimum) 
- Steel blocks (75x75x20mm for advanced testing)
- Test cutting tools (verified condition and runout)
- **Note:** Test coordinates assume 500x500x200mm travel envelope with 25mm safety margins

## Test Setup and Initialization

### Travel Envelope Configuration

**Default Configuration: 500x500x200mm**
- All test files are designed for CNC machines with **500x500x200mm travel limits**
- **Safety margins**: 25mm from each limit for safe operation
- **Usable range**: X25-475mm, Y25-475mm, Z5-195mm
- **Center point**: X250, Y250 for workpiece positioning

### Custom Travel Limits

If your machine has different travel limits, use the provided G-code generator:

```bash
# Generate test files for your specific machine
python generate_test_gcode.py [x_max] [y_max] [z_max]

# Examples:
python generate_test_gcode.py 300 300 150    # Smaller machine
python generate_test_gcode.py 800 400 200    # Larger machine
python generate_test_gcode.py 1000 1000 300  # Large format machine
```

**Generator Features:**
- Automatically scales coordinates to your travel limits
- Maintains safety margins and proportional test patterns
- Generates optimized test files for your specific envelope
- Creates configuration documentation
- Preserves test methodology while adapting to machine size

### Initial Machine State Verification
```gcode
; Pre-test machine verification
G28                 ; Home all axes
G90 G94             ; Absolute positioning, feed rate mode
M208                ; Reset S-curve to defaults
M5                  ; Ensure spindle off
M9                  ; Coolant off
G54                 ; Select work coordinate system
M207                ; Report initial S-curve settings
$$                  ; Display all settings for documentation
```

### Baseline Parameter Setup
```gcode
; Conservative baseline for testing
M204 P6000 T3000    ; Moderate acceleration for testing
M205 X250 Z125      ; Conservative jerk starting point
M206 M1.0 C0.7 A1   ; Standard settings with adaptive enabled
M210 F1.2 J1.0 A120 ; Moderate junction optimization
M211 S0             ; Disable path blending initially
M212 V30.0 Q2.0 D1.0 ; Standard final deceleration
```

## Test 1: Basic S-Curve Functionality Verification

**Purpose:** Verify S-curve is operational and compare to linear acceleration
**Duration:** 10 minutes
**Required:** No cutting, positioning only

### Test 1A: Linear vs S-Curve Motion Comparison

Save as `test_1a_scurve_verification.gcode`:
```gcode
; Test 1A: S-curve vs Linear Motion Comparison
; Verifies S-curve system is functioning correctly
; Designed for 500x500x200mm travel envelope

M208                ; Reset to defaults
G28                 ; Home all axes
G90 G94             ; Absolute, feed rate mode

; Test 1: Very low jerk (approximates linear)
M205 X25 Z15        ; Minimal jerk settings
G1 F1000 X150 Y150  ; Move to start position (uses central area)
G4 P2               ; Pause for observation

; Execute test pattern with minimal S-curve - 300mm square
G1 F2500 X450 Y150  ; Fast move in X (300mm travel)
G4 P1
G1 Y450             ; Fast move in Y (300mm travel)  
G4 P1
G1 X150             ; Return move (300mm travel)
G4 P1
G1 Y150             ; Complete square (300mm travel)
G4 P2

; Test 2: Proper S-curve settings
M205 X300 Z150      ; Proper S-curve jerk values
M206 M1.2 C0.7      ; Enable full S-curve features
G4 P2               ; Pause for comparison

; Execute same pattern with full S-curve
G1 F2500 X450 Y150  ; Same moves with S-curve enabled
G4 P1
G1 Y450
G4 P1  
G1 X150
G4 P1
G1 Y150
G4 P2

; Return to home
G28
M207                ; Report final settings
```

**Observation Checklist:**
- [ ] First pattern: More abrupt motion, potential vibration
- [ ] Second pattern: Smoother acceleration/deceleration transitions  
- [ ] No missed steps or position errors
- [ ] Audible difference in motor sound characteristics
- [ ] Visual difference in motion smoothness

### Test 1B: Parameter Response Verification

Save as `test_1b_parameter_response.gcode`:
```gcode
; Test 1B: Verify parameter changes take effect
; Tests immediate response to parameter modifications
; Designed for 500x500x200mm travel envelope

G28 G90 G94
G1 F1000 X250 Y250  ; Move to center for testing

; Test 1: Low jerk response
M205 X50 Z25        ; Low jerk settings
M207                ; Report settings
G1 F2000 X400 Y250  ; 150mm move
G4 P2
G1 X250             ; Return move

; Test 2: Medium jerk response  
M205 X200 Z100      ; Medium jerk settings
M207                ; Report settings
G1 Y400             ; 150mm move  
G4 P2
G1 Y250             ; Return move

; Test 3: High jerk response
M205 X500 Z250      ; High jerk settings
M207                ; Report settings
G1 X400             ; 150mm move
G4 P2
G1 X250             ; Return move

; Test 4: Multiplier effect
M205 X300 Z150      ; Reset to medium
M206 M0.5           ; Low multiplier
G1 Y400
G4 P1
G1 Y250
M206 M2.0           ; High multiplier  
G1 X400
G4 P1
G1 X250

; Reset and return home
M208
G28
```

**Expected Results:**
- [ ] Clearly distinguishable motion characteristics between settings
- [ ] Low jerk: very smooth, potentially slower transitions
- [ ] High jerk: more responsive, potentially some vibration
- [ ] Multiplier effect: noticeable scaling of jerk behavior
- [ ] All moves complete accurately without position loss

## Test 2: Comprehensive Parameter Testing

**Purpose:** Systematically test all available S-curve parameters
**Duration:** 45 minutes
**Required:** No cutting, comprehensive parameter evaluation

### Test 2A: Jerk Value Optimization ($800-$805)

Save as `test_2a_jerk_optimization.gcode`:
```gcode
; Test 2A: Systematic Jerk Value Testing
; Tests jerk values from conservative to aggressive
; Designed for 500x500x200mm travel envelope

G28 G90 G94
M208                ; Reset to defaults

; Define test pattern (complex geometry to stress jerk limits)
; Move to start position
G1 F1000 X100 Y100

; Test 1: Very Conservative Jerk (NEMA17 equivalent)
M205 X150 Z80       ; Conservative baseline
M207                ; Report settings
; Complex test pattern using larger coordinates
G1 F3000 X250 Y100  ; Fast linear move (150mm)
G1 X250 Y300        ; 90-degree corner (200mm)
G1 X100 Y300        ; Another corner (150mm)
G1 X100 Y200        ; Shorter move (100mm)
G1 X175 Y200        ; Short move with direction change (75mm)
G1 X175 Y100        ; Return to start (100mm)
G4 P3               ; Pause for evaluation

; Test 2: NEMA23 Conservative
M205 X250 Z125      ; Higher torque motor capability
M207                ; Report settings
; Same pattern scaled for larger machine
G1 F3000 X250 Y100
G1 X250 Y300
G1 X100 Y300
G1 X100 Y200
G1 X175 Y200
G1 X175 Y100
G4 P3

; Test 3: NEMA23 Moderate
M205 X350 Z175      ; Moderate settings
M207
G1 F3000 X250 Y100
G1 X250 Y300
G1 X100 Y300
G1 X100 Y200
G1 X175 Y200
G1 X175 Y100
G4 P3

; Test 4: NEMA23/24 Aggressive
M205 X500 Z250      ; Aggressive for high-torque steppers
M207
G1 F3000 X250 Y100
G1 X250 Y300
G1 X100 Y300
G1 X100 Y200
G1 X175 Y200
G1 X175 Y100
G4 P3

; Test 5: Servo System Aggressive
M205 X700 Z350      ; Servo motor capabilities
M207
G1 F3000 X250 Y100
G1 X250 Y300
G1 X100 Y300
G1 X100 Y200
G1 X175 Y200
G1 X175 Y100
G4 P3

G28
```

**Evaluation Criteria:**
- [ ] Record vibration level (1-10 scale) for each test
- [ ] Note any missed steps or position errors
- [ ] Measure cycle time for each test sequence
- [ ] Assess motion smoothness subjectively
- [ ] Find highest jerk values before problems appear

### Test 2B: S-Curve Advanced Parameters ($772-$777)

Save as `test_2b_advanced_parameters.gcode`:
```gcode
; Test 2B: S-curve Advanced Parameter Testing
; Tests multiplier, corner factor, and adaptive jerk

G28 G90 G94
M205 X350 Z175      ; Set moderate base jerk for testing

; Test 1: Jerk Multiplier Effects ($772)
M206 M0.7 C0.7 A0   ; Conservative multiplier, adaptive off
M207
G1 F2500 X25 Y25
; Test pattern with sharp corners
G1 X75 Y25
G1 X75 Y75
G1 X25 Y75
G1 X25 Y25
G4 P2

M206 M1.0 C0.7 A0   ; Standard multiplier
M207
G1 X75 Y25
G1 X75 Y75
G1 X25 Y75
G1 X25 Y25
G4 P2

M206 M1.4 C0.7 A0   ; Aggressive multiplier
M207
G1 X75 Y25
G1 X75 Y75
G1 X25 Y75
G1 X25 Y25
G4 P2

; Test 2: Corner Factor Effects ($773)
M206 M1.2 C0.3 A0   ; Aggressive corner reduction
M207
G1 X75 Y25
G1 X75 Y75
G1 X25 Y75
G1 X25 Y25
G4 P2

M206 M1.2 C0.7 A0   ; Moderate corner reduction
M207
G1 X75 Y25
G1 X75 Y75
G1 X25 Y75
G1 X25 Y25
G4 P2

M206 M1.2 C1.0 A0   ; No corner reduction
M207
G1 X75 Y25
G1 X75 Y75
G1 X25 Y75
G1 X25 Y25
G4 P2

; Test 3: Adaptive Jerk Comparison ($774)
M206 M1.2 C0.7 A0   ; Adaptive off
M207
; Mixed move lengths to test adaptive response
G1 F3000 X100 Y25   ; Long move (75mm)
G1 X105 Y30         ; Short move (7mm)
G1 X100 Y75         ; Medium move (45mm)
G1 X102 Y77         ; Very short move (3mm)
G1 X25 Y75          ; Long move (77mm)
G1 X25 Y25          ; Medium move (50mm)
G4 P2

M206 M1.2 C0.7 A1   ; Adaptive on
M207
; Same pattern - should auto-adjust jerk
G1 F3000 X100 Y25
G1 X105 Y30
G1 X100 Y75
G1 X102 Y77
G1 X25 Y75
G1 X25 Y25
G4 P2

G28
```

**Evaluation Criteria:**
- [ ] **Multiplier test:** Note smoothness vs responsiveness trade-off
- [ ] **Corner factor test:** Measure corner rounding vs overshoot
- [ ] **Adaptive test:** Compare motion consistency between patterns
- [ ] Record optimal values for each parameter

### Test 2C: Junction Optimization Parameters ($775-$777)

Save as `test_2c_junction_optimization.gcode`:
```gcode
; Test 2C: Junction Velocity Optimization Testing
; Tests junction-specific parameters

G28 G90 G94
M205 X400 Z200      ; Set aggressive base jerk for junction testing
M206 M1.2 C0.7 A1   ; Standard settings

; Test 1: Junction Velocity Factor ($775)
M210 F0.8 J1.0 A120 ; Conservative junction velocity
M207
; Junction-intensive pattern
G1 F4000 X50 Y25
G1 X75 Y50          ; 45-degree angle
G1 X100 Y25         ; Sharp corner
G1 X100 Y75         ; 90-degree corner
G1 X75 Y100         ; 45-degree angle
G1 X25 Y100         ; Sharp corner
G1 X25 Y25          ; 90-degree corner
G1 X50 Y25          ; Complete pattern
G4 P3

M210 F1.2 J1.0 A120 ; Moderate junction velocity
M207
; Same pattern
G1 F4000 X75 Y50
G1 X100 Y25
G1 X100 Y75
G1 X75 Y100
G1 X25 Y100
G1 X25 Y25
G1 X50 Y25
G4 P3

M210 F1.6 J1.0 A120 ; Aggressive junction velocity
M207
; Same pattern
G1 F4000 X75 Y50
G1 X100 Y25
G1 X100 Y75
G1 X75 Y100
G1 X25 Y100
G1 X25 Y25
G1 X50 Y25
G4 P3

; Test 2: Junction Jerk Multiplier ($776)
M210 F1.2 J0.7 A120 ; Conservative junction jerk
M207
; Rapid direction changes
G1 F3000 X75 Y25
G1 X75 Y75          ; 180-degree reversal
G1 X50 Y75
G1 X50 Y25          ; 180-degree reversal
G1 X75 Y25
G4 P2

M210 F1.2 J1.0 A120 ; Standard junction jerk
M207
G1 F3000 X75 Y75
G1 X50 Y75
G1 X50 Y25
G1 X75 Y25
G4 P2

M210 F1.2 J1.4 A120 ; Aggressive junction jerk
M207
G1 F3000 X75 Y75
G1 X50 Y75
G1 X50 Y25
G1 X75 Y25
G4 P2

; Test 3: Junction Angle Threshold ($777)
M210 F1.2 J1.0 A180 ; All angles get junction treatment
M207
; Various angle test
G1 F3000 X60 Y25    ; Start
G1 X80 Y35          ; ~30 degree angle
G1 X100 Y25         ; ~30 degree angle
G1 X100 Y45         ; 90 degree angle
G1 X80 Y65          ; ~30 degree angle
G1 X60 Y75          ; ~30 degree angle
G1 X25 Y75          ; Sharp angle
G1 X25 Y25          ; 90 degree angle
G4 P2

M210 F1.2 J1.0 A90  ; Only sharp angles get treatment
M207
; Same pattern
G1 F3000 X80 Y35
G1 X100 Y25
G1 X100 Y45
G1 X80 Y65
G1 X60 Y75
G1 X25 Y75
G1 X25 Y25
G1 X60 Y25
G4 P2

G28
```

**Evaluation Criteria:**
- [ ] **Velocity factor:** Measure cycle time vs corner accuracy
- [ ] **Jerk multiplier:** Assess smoothness of direction changes
- [ ] **Angle threshold:** Note behavior differences at various angles
- [ ] Find optimal balance of speed vs precision

### Test 2D: Path Blending Parameters ($778-$783)

Save as `test_2d_path_blending.gcode`:
```gcode
; Test 2D: Path Blending Parameter Testing
; Tests all path blending parameters systematically

G28 G90 G94
M205 X400 Z200      ; Set good base jerk for blending tests
M206 M1.2 C0.7 A1   ; Standard S-curve settings

; Test 1: Path Blending Enable/Disable ($778)
M211 S0             ; Disable path blending
M207
; Complex curved path that benefits from blending
G1 F3000 X25 Y25
G1 X30 Y40
G1 X40 Y50
G1 X55 Y55
G1 X70 Y50
G1 X80 Y40
G1 X85 Y25
G1 X80 Y10
G1 X70 Y5
G1 X55 Y5
G1 X40 Y10
G1 X30 Y20
G1 X25 Y25
G4 P3               ; Pause to observe non-blended motion

M211 S1 P0.02 R2.0 V50 F0.7 L8  ; Enable with standard settings
M207
; Same path with blending enabled
G1 F3000 X30 Y40
G1 X40 Y50
G1 X55 Y55
G1 X70 Y50
G1 X80 Y40
G1 X85 Y25
G1 X80 Y10
G1 X70 Y5
G1 X55 Y5
G1 X40 Y10
G1 X30 Y20
G1 X25 Y25
G4 P3

; Test 2: Blending Tolerance ($779)
M211 S1 P0.001 R2.0 V50 F0.7 L8  ; Very tight tolerance
M207
G1 F3000 X30 Y40
G1 X40 Y50
G1 X55 Y55
G1 X70 Y50
G1 X80 Y40
G1 X85 Y25
G4 P2

M211 S1 P0.05 R2.0 V50 F0.7 L8   ; Loose tolerance
M207
G1 F3000 X80 Y10
G1 X70 Y5
G1 X55 Y5
G1 X40 Y10
G1 X30 Y20
G1 X25 Y25
G4 P2

; Test 3: Max Blend Radius ($780)
M211 S1 P0.02 R1.0 V50 F0.7 L8   ; Small radius
M207
; Sharp corner test
G1 F2500 X40 Y40
G1 X60 Y40          ; Sharp 90-degree corner
G1 X60 Y60
G1 X40 Y60
G1 X40 Y40
G4 P2

M211 S1 P0.02 R5.0 V50 F0.7 L8   ; Large radius
M207
; Same sharp corners
G1 F2500 X60 Y40
G1 X60 Y60
G1 X40 Y60
G1 X40 Y40
G4 P2

; Test 4: Min Blend Velocity ($781)
M211 S1 P0.02 R2.0 V10 F0.7 L8   ; Low minimum velocity
M207
; Mixed speed test
G1 F500 X50 Y40     ; Slow move - should still blend
G1 F3000 X50 Y60    ; Fast move - definitely blends
G1 F200 X40 Y60     ; Very slow - may not blend
G1 F2000 X40 Y40    ; Medium speed
G4 P2

M211 S1 P0.02 R2.0 V200 F0.7 L8  ; High minimum velocity
M207
; Same pattern - slow moves should not blend
G1 F500 X50 Y60
G1 F3000 X40 Y60
G1 F200 X40 Y40
G1 F2000 X50 Y40
G4 P2

; Test 5: Blend Jerk Factor ($782)
M211 S1 P0.02 R2.0 V50 F0.3 L8   ; Very conservative blending
M207
G1 F3000 X60 Y50
G1 X80 Y70
G1 X60 Y90
G1 X40 Y70
G1 X60 Y50
G4 P2

M211 S1 P0.02 R2.0 V50 F1.0 L8   ; Aggressive blending
M207
G1 F3000 X80 Y70
G1 X60 Y90
G1 X40 Y70
G1 X60 Y50
G4 P2

; Test 6: Lookahead Blocks ($783)
M211 S1 P0.02 R2.0 V50 F0.7 L3   ; Minimal lookahead
M207
; Many small segments to test lookahead
G1 F2000 X45 Y45
G1 X46 Y47
G1 X48 Y48
G1 X51 Y48
G1 X54 Y47
G1 X55 Y45
G1 X54 Y43
G1 X51 Y42
G1 X48 Y42
G1 X46 Y43
G1 X45 Y45
G4 P2

M211 S1 P0.02 R2.0 V50 F0.7 L16  ; Maximum lookahead
M207
; Same segment pattern - should be much smoother
G1 F2000 X46 Y47
G1 X48 Y48
G1 X51 Y48
G1 X54 Y47
G1 X55 Y45
G1 X54 Y43
G1 X51 Y42
G1 X48 Y42
G1 X46 Y43
G1 X45 Y45
G4 P2

G28
```

**Evaluation Criteria:**
- [ ] **Enable/Disable:** Note smoothness difference between blended and non-blended
- [ ] **Tolerance:** Measure actual vs programmed path deviation
- [ ] **Blend radius:** Observe corner rounding at different radii
- [ ] **Min velocity:** Confirm blending activation thresholds
- [ ] **Jerk factor:** Assess smoothness vs responsiveness in blended paths
- [ ] **Lookahead:** Note optimization quality with different block counts

### Test 2E: Final Deceleration Parameters ($784-$786)

Save as `test_2e_final_deceleration.gcode`:
```gcode
; Test 2E: Final Deceleration Optimization Testing
; Tests the "jog crawling" fix parameters

G28 G90 G94
M205 X400 Z200      ; Good base jerk for testing

; Test 1: Min Stop Velocity ($784)
M212 V10.0 Q2.0 D1.0 ; Very low min stop velocity
M207
; Jog-style moves that would normally crawl
G1 F2000 X100       ; 100mm move
G4 P1
G1 X50              ; Return - check final 10mm approach
G4 P2

M212 V100.0 Q2.0 D1.0 ; High min stop velocity
M207
G1 F2000 X100       ; Same move
G4 P1
G1 X50              ; Should switch to linear decel sooner
G4 P2

; Test 2: Final Decel Jerk Multiplier ($785)
M212 V40.0 Q1.0 D1.0 ; Conservative final jerk
M207
G1 F2000 Y100       ; 100mm move
G4 P1
G1 Y50              ; Gentle final approach
G4 P2

M212 V40.0 Q4.0 D1.0 ; Aggressive final jerk
M207
G1 F2000 Y100       ; Same move
G4 P1
G1 Y50              ; Crisp final approach
G4 P2

; Test 3: Stop Threshold Distance ($786)
M212 V40.0 Q2.5 D0.5 ; Small threshold
M207
G1 F2000 X100       ; 100mm move
G4 P1
G1 X50              ; Enhanced stopping only in final 0.5mm
G4 P2

M212 V40.0 Q2.5 D5.0 ; Large threshold
M207
G1 F2000 X100       ; Same move
G4 P1
G1 X50              ; Enhanced stopping starts 5mm from target
G4 P2

; Test 4: Real Jog Simulation
; Test various jog distances with optimized settings
M212 V50.0 Q3.0 D1.5 ; Optimized for CNC machines
M207

; Short jogs
G1 F1000 X55        ; 5mm jog
G1 X60              ; 5mm jog
G1 X55              ; Return
G4 P1

; Medium jogs
G1 F1500 X75        ; 20mm jog
G1 X55              ; Return
G4 P1

; Long jogs
G1 F2000 X100       ; 45mm jog
G1 X55              ; Return
G4 P1

; Z-axis jogs (often more problematic)
G1 F800 Z20         ; 20mm Z jog
G1 Z5               ; Return
G4 P1

G28
```

**Evaluation Criteria:**
- [ ] **Min stop velocity:** Time final approach, check for crawling
- [ ] **Final jerk multiplier:** Balance crispness vs overshoot
- [ ] **Stop threshold:** Optimal distance for enhanced deceleration
- [ ] **Jog simulation:** Real-world jog behavior validation
- [ ] **No crawling:** Final 2mm should complete in reasonable time

## Test 3: CNC Machining Performance Tests

**Purpose:** Validate S-curve performance with actual cutting operations
**Duration:** 60 minutes (setup + cutting tests)
**Required:** Cutting tools, test materials, safety equipment

### Test 3A: Surface Finish Validation with Actual Cutting

Save as `test_3a_surface_finish_cutting.gcode`:
```gcode
; Test 3A: Surface Finish Test with Actual Machining
; Creates test surfaces for roughness measurement with different S-curve settings
; REQUIRES: End mill, aluminum test block, proper speeds/feeds

; Safety and setup
G28                 ; Home all axes
M5                  ; Spindle off
M9                  ; Coolant off
G54                 ; Work coordinate system
G90 G94             ; Absolute positioning, feed rate mode

; Tool setup (USER MUST ADJUST FOR THEIR SETUP)
; G0 Z50            ; Retract to safe height
; T1 M6             ; Change to test tool (if ATC available)
; M3 S12000         ; Start spindle (ADJUST SPEED FOR YOUR TOOL)
; G4 P3             ; Spindle up delay

; Move to start position
G0 X10 Y10
G0 Z2
G1 Z-0.5 F200       ; Light cut depth

; Test Surface 1: Conservative S-curve settings
M205 X200 Z100      ; Conservative jerk for smoothness
M206 M0.9 C0.5 A1   ; Smooth motion priority
M207                ; Report settings

; Create 30x20mm test surface
G1 X40 Y10 F600     ; Finishing feed rate
G1 Y30
G1 X10
G1 Y10
; Label area for measurement
; (User should mark this as "Conservative")

G1 Z2               ; Retract

; Test Surface 2: Moderate S-curve settings  
M205 X350 Z175      ; Moderate jerk
M206 M1.2 C0.7 A1   ; Balanced settings
M207

; Move to next area
G0 X50 Y10
G1 Z-0.5 F200

; Create test surface
G1 X80 Y10 F600
G1 Y30
G1 X50
G1 Y10
; Label as "Moderate"

G1 Z2

; Test Surface 3: Aggressive S-curve settings
M205 X500 Z250      ; Aggressive jerk for speed
M206 M1.4 C0.8 A1   ; Speed priority
M207

; Move to next area
G0 X90 Y10
G1 Z-0.5 F200

; Create test surface
G1 X120 Y10 F600
G1 Y30
G1 X90
G1 Y10
; Label as "Aggressive"

; Safety shutdown
G1 Z10              ; Retract
M5                  ; Stop spindle
M9                  ; Coolant off
G28                 ; Home
```

**Required Measurements:**
- [ ] Surface roughness (Ra) for each test area using surface roughness gauge
- [ ] Visual inspection for tool marks and surface quality
- [ ] Dimensional accuracy of each test rectangle
- [ ] Note any tool chatter or vibration during cutting

**Expected Results:**
- Conservative: Best surface finish, potentially slower cycle time
- Moderate: Good balance of finish and speed  
- Aggressive: Fastest cycle time, may show slight finish degradation

### Test 3B: Corner Accuracy and Dimensional Precision

Save as `test_3b_corner_accuracy.gcode`:
```gcode
; Test 3B: Corner Accuracy Test with Actual Cutting
; Tests dimensional accuracy and corner quality with different S-curve settings
; REQUIRES: End mill, aluminum test block

; Setup (USER MUST CONFIGURE)
G28
G54 G90 G94
; M3 S12000         ; Start spindle (ADJUST FOR YOUR SETUP)
; G4 P3

; Move to start position
G0 X20 Y20
G0 Z2
G1 Z-0.2 F200       ; Very light cut for accuracy test

; Test 1: Conservative settings for maximum accuracy
M205 X250 Z125      ; Conservative jerk
M206 M1.0 C0.5 A1   ; Prioritize accuracy
M210 F1.0 J0.9 A150 ; Gentle junction handling
M211 S1 P0.005 R1.0 ; Tight blending tolerance
M207                ; Report settings

; Cut precision test square (20x20mm)
G1 X40 Y20 F300     ; Slow feed for accuracy
G1 Y40
G1 X20
G1 Y20
; Mark as "Accuracy Test"

G1 Z2               ; Retract

; Test 2: Aggressive settings for speed comparison
M205 X500 Z250      ; Aggressive jerk
M206 M1.4 C0.8 A1   ; Prioritize speed
M210 F1.5 J1.3 A90  ; Aggressive junctions
M211 S1 P0.02 R3.0  ; Loose blending
M207

; Move to next position
G0 X50 Y20
G1 Z-0.2 F200

; Cut speed test square (same size)
G1 X70 Y20 F300     ; Same feed rate for comparison
G1 Y40
G1 X50
G1 Y20
; Mark as "Speed Test"

; Safety shutdown
G1 Z10
M5
G28
```

**Required Measurements:**
- [ ] Measure both squares with calipers (should be 20.000 x 20.000mm)
- [ ] Check corner squareness with precision square
- [ ] Measure corner radius (should be minimal)
- [ ] Compare cycle times between tests
- [ ] Note any corner overshoot or rounding

### Test 3C: Tool Life and Cutting Force Evaluation

Save as `test_3c_tool_life_test.gcode`:
```gcode
; Test 3C: Tool Life and Cutting Force Test
; Extended cutting test to evaluate tool wear with different S-curve settings
; REQUIRES: Multiple identical end mills, extended test material

; This test should be run with different S-curve settings
; to compare tool wear patterns

G28 G54 G90 G94
; M3 S15000         ; Adjust for your setup

; Extended cutting pattern to stress tool
; USER: Adjust coordinates for your setup
G0 X10 Y10
G0 Z2

; Conservative S-curve for tool life
M205 X200 Z100      ; Smooth motion for tool longevity
M206 M0.9 C0.6      ; Gentle on tool
M212 V30.0 Q2.0     ; Smooth final approach

; Extended cutting pattern (slotting operation)
G1 Z-1.0 F200       ; 1mm depth
G1 Y50 F400         ; Slot length
G1 X12 F100         ; Step over
G1 Y10 F400         ; Return pass
G1 X14 F100         ; Step over
G1 Y50 F400         ; Forward pass
G1 X16 F100         ; Continue pattern
G1 Y10 F400
G1 X18 F100
G1 Y50 F400
G1 X20 F100
G1 Y10 F400

G1 Z2               ; Retract
M5

; Stop here to inspect tool wear
; Record tool condition, then repeat with aggressive settings
G28
```

**Tool Wear Assessment:**
- [ ] Inspect cutting edge condition before/after each test
- [ ] Note any chipping or unusual wear patterns
- [ ] Record cutting time for each test
- [ ] Compare surface finish quality over extended cutting
- [ ] Assess tool life impact of different S-curve settings

## Test 4: Motor System Specific Tests

**Purpose:** Optimize S-curve settings for specific motor types
**Duration:** 30 minutes
**Required:** Knowledge of motor specifications

### Test 4A: NEMA23/24 Stepper Motor Optimization

Save as `test_4a_stepper_optimization.gcode`:
```gcode
; Test 4A: NEMA23/24 Stepper Motor Optimization
; Tests motor-specific capabilities and limits

G28 G90 G94
M208                ; Reset to defaults

; Test 1: Baseline stepper performance
M205 X300 Z150      ; Good starting point for NEMA23
M206 M1.1 C0.7 A1   ; Moderate settings
M207

; Stepper torque curve test (varying speeds)
G1 F500 X50         ; Low speed (high torque region)
G1 F1500 Y50        ; Medium speed
G1 F3000 X0         ; Higher speed (torque dropoff region)
G1 F500 Y0          ; Return to low speed
G4 P2

; Test 2: Progressive jerk increase to find stepper limits
M205 X400 Z200      ; Increase jerk
G1 F2000 X50 Y50
G1 X0 Y0
G4 P1

M205 X500 Z250      ; Further increase
G1 F2000 X50 Y50
G1 X0 Y0
G4 P1

M205 X600 Z300      ; Near stepper limits
G1 F2000 X50 Y50
G1 X0 Y0
G4 P1

M205 X700 Z350      ; Likely beyond safe limits
G1 F2000 X50 Y50
G1 X0 Y0
; Listen for missed steps or irregular motion
G4 P1

; Test 3: High-frequency direction changes (stepper challenge)
M205 X400 Z200      ; Back to known good setting
G1 F1500 X10
G1 X20
G1 X10
G1 X20
G1 X10
G1 X20
G1 X10              ; Rapid reversals stress steppers

G28
```

**Stepper-Specific Evaluation:**
- [ ] Note maximum jerk before missed steps
- [ ] Listen for changes in motor sound/vibration
- [ ] Check position accuracy after high-speed tests
- [ ] Find optimal jerk for stepper torque curve
- [ ] Document maximum reliable settings

### Test 4B: Servo Motor Advantage Testing  

Save as `test_4b_servo_optimization.gcode`:
```gcode
; Test 4B: Servo Motor Optimization Test
; Takes advantage of servo feedback and higher performance

G28 G90 G94
M208

; Test 1: Aggressive settings possible with servos
M205 X600 Z300      ; Start with higher values than steppers
M206 M1.4 C0.8 A1   ; Aggressive settings
M207

; Servo precision test
G1 F3000 X50 Y50
G1 X0 Y0
G4 P1

; Test 2: Push servo limits
M205 X800 Z400      ; Very aggressive
G1 F4000 X50 Y50
G1 X0 Y0
G4 P1

M205 X1000 Z500     ; Extreme settings (may be too much)
G1 F4000 X50 Y50
G1 X0 Y0
G4 P1

; Test 3: Complex path with servo advantages
M205 X700 Z350      ; Optimized for servos
M206 M1.5 C0.8 A1   ; Take full advantage
M210 F1.5 J1.4 A80  ; Aggressive junction optimization
M211 S1 P0.01 R3.0 V100 F0.8 L16  ; Advanced path blending

; Complex servo stress test
G1 F5000 X25 Y25
G1 X30 Y45
G1 X45 Y55
G1 X65 Y50
G1 X75 Y30
G1 X70 Y10
G1 X50 Y5
G1 X30 Y15
G1 X25 Y25
; Smooth, high-speed complex motion only possible with servos

G28
```

**Servo-Specific Evaluation:**
- [ ] Test much higher jerk values than steppers
- [ ] Verify position accuracy maintained at high speeds
- [ ] Confirm no following errors during complex motion
- [ ] Document servo-specific optimal settings
- [ ] Compare performance to stepper limitations

## Test 5: Material-Specific Optimization

**Purpose:** Optimize S-curve settings for different CNC materials
**Duration:** 90 minutes (multiple material tests)
**Required:** Different material samples

### Test 5A: Aluminum Machining Optimization

Save as `test_5a_aluminum_optimization.gcode`:
```gcode
; Test 5A: Aluminum Machining Optimization
; Optimizes S-curve for aluminum cutting characteristics

; SETUP REQUIRED: 6061 aluminum, appropriate end mill, speeds/feeds
G28 G54 G90 G94
; M3 S18000         ; High speed for aluminum (ADJUST)

; Aluminum allows aggressive settings due to:
; - Easy machining characteristics
; - Good heat dissipation  
; - Minimal tool chatter concerns

; Test 1: Aggressive aluminum settings
M205 X500 Z250      ; High jerk for aluminum
M206 M1.4 C0.8 A1   ; Aggressive motion
M210 F1.5 J1.3 A90  ; Fast cornering
M211 S1 P0.02 R3.0 V75 F0.8 L10  ; Optimized blending
M212 V60.0 Q3.0 D2.0 ; Quick positioning

G0 X20 Y20
G0 Z2
G1 Z-2.0 F400       ; Deeper cut possible in aluminum

; High-speed aluminum roughing simulation
G1 X80 Y20 F1200    ; High feed rate
G1 Y30 F300         ; Plunge rate
G1 X20 F1200        ; Rapid traverse
G1 Y40 F300
G1 X80 F1200
G1 Y50 F300
G1 X20 F1200

; Aluminum finishing pass
G1 X80 Y50 F800     ; Finishing feed
G1 Y20
G1 X20
G1 Y50

G1 Z5
M5
G28
```

**Aluminum-Specific Metrics:**
- [ ] Surface finish quality at high feed rates
- [ ] Corner sharpness with aggressive settings
- [ ] Cycle time improvements vs quality
- [ ] Tool life with optimized motion
- [ ] Chip evacuation effectiveness

### Test 5B: Steel Machining Optimization

Save as `test_5b_steel_optimization.gcode`:
```gcode
; Test 5B: Steel Machining Optimization  
; Conservative S-curve settings for steel cutting

; SETUP: Steel block, carbide end mill, appropriate speeds/feeds
G28 G54 G90 G94
; M3 S8000          ; Lower speed for steel (ADJUST)

; Steel requires conservative settings due to:
; - Higher cutting forces
; - Tool chatter concerns
; - Heat generation
; - Tool wear considerations

; Test 1: Conservative steel settings
M205 X300 Z150      ; Moderate jerk to prevent chatter
M206 M1.1 C0.6 A1   ; Smooth motion priority
M210 F1.2 J1.0 A120 ; Gentle cornering
M211 S1 P0.015 R2.0 V40 F0.6 L8  ; Careful blending
M212 V35.0 Q2.2 D1.2 ; Controlled positioning

G0 X20 Y20
G0 Z2
G1 Z-0.8 F150       ; Lighter cut in steel

; Steel roughing pass
G1 X60 Y20 F300     ; Conservative feed rate
G1 Y25 F100         ; Slow plunge
G1 X20 F300
G1 Y30 F100
G1 X60 F300
G1 Y35 F100
G1 X20 F300

; Steel finishing pass
G1 X60 Y35 F200     ; Slow finishing feed
G1 Y20
G1 X20
G1 Y35

G1 Z5
M5
G28
```

**Steel-Specific Metrics:**
- [ ] Tool chatter prevention effectiveness
- [ ] Surface finish quality with conservative settings
- [ ] Tool life extension vs cycle time
- [ ] Heat generation control
- [ ] Dimensional accuracy maintenance

## Test 6: Production Validation Tests

**Purpose:** Real-world validation with actual production parts
**Duration:** 2-4 hours (depends on part complexity)
**Required:** Actual production G-code

### Test 6A: Production Part Comparison

```gcode
; Test 6A: Production Part Validation
; Run actual production parts with different S-curve settings

; This test requires user's actual production G-code
; Run the same part with different S-curve settings:

; Setting 1: Conservative (baseline)
M205 X250 Z125
M206 M1.0 C0.6 A1
M210 F1.1 J1.0 A130
M211 S1 P0.02 R2.0 V40 F0.6 L6
; Run production part and record:
; - Cycle time
; - Surface finish
; - Dimensional accuracy
; - Tool condition after run

; Setting 2: Optimized (from testing)
; Use settings determined from previous tests
; Run same part and compare results

; Setting 3: Aggressive (maximum performance)  
; Push settings to maximum reliable values
; Run part and validate still within tolerances
```

**Production Validation Metrics:**
- [ ] **Cycle Time:** % improvement over baseline
- [ ] **Surface Finish:** Ra measurements on critical surfaces
- [ ] **Dimensional Accuracy:** Critical dimensions within tolerance
- [ ] **Tool Life:** Wear rate comparison between settings
- [ ] **Machine Vibration:** Any increase in structural stress
- [ ] **Power Consumption:** Motor current/power differences
- [ ] **Consistency:** Part-to-part variation

## Test Results Analysis and Documentation

### Performance Metrics Summary Table

Create a comprehensive table documenting all test results:

| Parameter | Conservative | Moderate | Aggressive | Optimal |
|-----------|-------------|----------|------------|---------|
| X/Y Jerk (mm/sec³) | | | | |
| Z Jerk (mm/sec³) | | | | |
| Multiplier | | | | |
| Corner Factor | | | | |
| Junction Velocity Factor | | | | |
| Junction Jerk Multiplier | | | | |
| Path Blending Tolerance | | | | |
| **Performance Results** | | | | |
| Cycle Time (sec) | | | | |
| Surface Finish Ra (μm) | | | | |
| Corner Accuracy (mm) | | | | |
| Tool Life (parts) | | | | |
| Vibration Level (1-10) | | | | |

### Optimization Decision Matrix

**Material-Specific Recommendations:**

**Aluminum:**
- Optimal Jerk: _____ mm/sec³
- Multiplier: _____
- Junction Settings: F_____ J_____ A_____
- Path Blending: P_____ R_____ V_____

**Steel:**
- Optimal Jerk: _____ mm/sec³  
- Multiplier: _____
- Junction Settings: F_____ J_____ A_____
- Path Blending: P_____ R_____ V_____

**Production Settings (Final):**
```
$800=____    ; X-axis jerk
$801=____    ; Y-axis jerk  
$802=____    ; Z-axis jerk
$772=____    ; S-curve multiplier
$773=____    ; Corner factor
$774=1       ; Adaptive enable
$775=____    ; Junction velocity factor
$776=____    ; Junction jerk multiplier
$777=____    ; Junction angle threshold
$778=1       ; Path blending enable
$779=____    ; Path blending tolerance
$780=____    ; Path blending max radius
$781=____    ; Path blending min velocity
$782=____    ; Path blending jerk factor
$783=____    ; Path blending lookahead
$784=____    ; Min stop velocity
$785=____    ; Final decel jerk multiplier
$786=____    ; Stop threshold distance
```

## Troubleshooting Test Failures

### Common Test Failure Modes

**Test 1 Failures:**
- **No difference between settings:** S-curve not enabled, check configuration
- **Missed steps:** Jerk values too high for motor capability
- **Controller errors:** Parameter out of range, check limits

**Test 2 Failures:**
- **Parameter changes ineffective:** Settings not updating, check M-code implementation
- **Erratic motion:** Mechanical problems, check machine condition
- **Position errors:** Acceleration/jerk mismatch, verify motor tuning

**Test 3 Failures:**
- **Poor surface finish:** Settings too aggressive, reduce jerk values
- **Tool chatter:** Insufficient vibration damping, reduce corner factor
- **Dimensional errors:** Path blending too loose, tighten tolerance

**Recovery Procedures:**

1. **Emergency situations:**
   ```gcode
   M208              ; Reset all S-curve parameters
   M5                ; Stop spindle
   G28               ; Home machine
   ```

2. **Progressive recovery:**
   - Return to last known good settings
   - Reduce all jerk values by 50%
   - Disable advanced features temporarily
   - Restart testing with conservative values

3. **Mechanical verification:**
   - Check for loose components
   - Verify motor current settings
   - Inspect tool condition and runout
   - Confirm work holding security

## Test Completion Checklist

Before putting optimized S-curve settings into production:

- [ ] **All basic tests passed** (Tests 1-2)
- [ ] **Material-specific optimization complete** (Test 5)
- [ ] **Production validation successful** (Test 6) 
- [ ] **Settings documented and saved** permanently
- [ ] **Backup of previous settings** created
- [ ] **Tool life impact** assessed and acceptable
- [ ] **Surface finish requirements** met
- [ ] **Dimensional accuracy** maintained
- [ ] **Cycle time improvements** quantified
- [ ] **Machine condition** verified throughout testing
- [ ] **Safety procedures** followed and effective

## Final Recommendations

### Implementation Strategy

1. **Phase 1:** Implement conservative optimized settings
2. **Phase 2:** Monitor production for 1 week, collect data
3. **Phase 3:** Gradually implement more aggressive settings if Phase 2 successful
4. **Phase 4:** Full optimization deployment with continued monitoring

### Ongoing Monitoring

**Weekly Checks:**
- [ ] Surface finish spot checks
- [ ] Tool life tracking
- [ ] Dimensional accuracy verification
- [ ] Machine vibration assessment

**Monthly Reviews:**
- [ ] Performance metrics trending
- [ ] Settings optimization opportunities
- [ ] Tool cost analysis
- [ ] Overall productivity improvements

This comprehensive testing guide ensures thorough validation of S-curve acceleration performance specifically for CNC machining applications on higher-end machines with NEMA23/24 steppers or servo motors.

