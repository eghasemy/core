# S-Curve Testing and Tuning Guide

This guide provides comprehensive testing procedures and G-code files to validate and optimize S-curve acceleration performance. Use these tests to find optimal settings for your specific machine and application.

## Overview

The testing process involves:
1. **Baseline Tests** - Verify basic S-curve functionality
2. **Performance Tests** - Measure speed and quality improvements
3. **Edge Case Tests** - Validate behavior in challenging scenarios
4. **Application Tests** - Real-world validation for your use case
5. **Stress Tests** - Push limits to find boundaries

## Prerequisites

Before testing:
- [ ] S-curve acceleration enabled and configured
- [ ] Machine mechanically sound (tight belts, lubricated bearings)
- [ ] Controller communication stable
- [ ] Safety equipment in place

## Test Setup

### Required Tools
- Terminal/console access to controller
- Measuring tools (calipers, rulers)
- Surface roughness gauge (optional but recommended)
- Timer or stopwatch
- Camera for motion recording (optional)

### Initial Configuration
```gcode
; Reset to known state
M208                ; Reset S-curve to defaults
G28                 ; Home all axes
G90                 ; Absolute positioning
G94                 ; Feed rate in units/min
M207                ; Report initial settings
```

## Test 1: Basic Functionality Test

**Purpose:** Verify S-curve is working and compare to linear acceleration

**Duration:** 5 minutes

### Test 1A: Linear vs S-Curve Comparison

Save as `test_1a_comparison.gcode`:
```gcode
; Test 1A: S-curve vs Linear Comparison
; This test compares motion with and without S-curve

M208                ; Reset to defaults
G28                 ; Home
G90 G94             ; Absolute, feed rate mode

; Test without S-curve (disable if possible, or use minimal jerk)
M205 X5 Z2          ; Very low jerk approximates linear
G1 F1000 X50 Y50    ; Move to start position
G4 P1               ; Pause 1 second

; Execute test pattern
G1 F3000 X100 Y0    ; Fast move in X
G1 Y100             ; Fast move in Y  
G1 X0               ; Fast move back
G1 Y0               ; Return to start

; Now test with proper S-curve
M205 X150 Z80       ; Standard S-curve settings
G4 P2               ; Pause
G1 F3000 X100 Y0    ; Same pattern with S-curve
G1 Y100
G1 X0
G1 Y0

; Return home
G28
```

**What to observe:**
- Smoother motion with S-curve enabled
- Less vibration and noise
- Potentially different timing

### Test 1B: Basic Parameter Response

Save as `test_1b_parameters.gcode`:
```gcode
; Test 1B: Parameter Response Test
; Tests if parameter changes take effect

G28 G90 G94
G1 F1000 X25 Y25    ; Move to start

; Test 1: Low jerk (should be very smooth)
M205 X10 Z5
G1 F2000 X75 Y25
G4 P1

; Test 2: Medium jerk
M205 X50 Z25  
G1 Y75
G4 P1

; Test 3: High jerk (should be more responsive)
M205 X200 Z100
G1 X25
G4 P1

; Reset and return
M208
G28
```

**Expected results:**
- Noticeably different motion characteristics
- Low jerk: very smooth, potentially slower
- High jerk: more responsive, potential vibration

## Test 2: Performance Characterization

**Purpose:** Measure quantitative performance improvements

**Duration:** 15 minutes

### Test 2A: Speed Test

Save as `test_2a_speed.gcode`:
```gcode
; Test 2A: Speed Performance Test
; Measures time to complete complex path

M208 G28 G90 G94
M205 X100 Z50       ; Baseline settings

; Record start time manually or use timer
; Complex path test
G1 F4000 X10 Y10
G1 X90 Y10
G1 X90 Y90  
G1 X10 Y90
G1 X10 Y10

; Circle approximation (octagon)
G1 X50 Y10
G1 X78.54 Y21.46    ; 8-sided polygon
G1 X90 Y50
G1 X78.54 Y78.54
G1 X50 Y90
G1 X21.46 Y78.54
G1 X10 Y50
G1 X21.46 Y21.46
G1 X50 Y10

; Zig-zag pattern
G1 X10 Y20
G1 X90 Y30
G1 X10 Y40
G1 X90 Y50
G1 X10 Y60
G1 X90 Y70
G1 X10 Y80

G28
; Record end time
```

**Repeat this test with different jerk settings:**
1. Conservative: `M205 X50 Z25`
2. Standard: `M205 X150 Z80`
3. Aggressive: `M205 X300 Z150`

**Measurements:**
- Total time for each run
- Quality of corners and curves
- Any vibration or missed steps

### Test 2B: Surface Quality Test

Save as `test_2b_surface.gcode`:
```gcode
; Test 2B: Surface Quality Test
; Creates test surfaces for roughness measurement

M208 G28 G90 G94

; Test patch 1: Conservative settings
M205 X30 Z15
M206 M0.7 C0.5
G1 F1500 X10 Y10
; Create 20x20mm test patch
G1 X30 Y10
G1 X30 Y30
G1 X10 Y30
G1 X10 Y10

; Test patch 2: Standard settings  
M205 X150 Z80
M206 M1.0 C0.7
G1 X40 Y10
G1 X60 Y10
G1 X60 Y30
G1 X40 Y30
G1 X40 Y10

; Test patch 3: Aggressive settings
M205 X300 Z150
M206 M1.3 C0.9
G1 X70 Y10
G1 X90 Y10
G1 X90 Y30
G1 X70 Y30
G1 X70 Y10

G28
```

**Measurements:**
- Surface roughness (Ra) if gauge available
- Visual inspection of surface quality
- Corner sharpness and accuracy

## Test 3: Edge Case Testing

**Purpose:** Validate behavior in challenging scenarios

**Duration:** 20 minutes

### Test 3A: Micro-Moves Test

Save as `test_3a_micromoves.gcode`:
```gcode
; Test 3A: Micro-Moves Test
; Tests very small moves where S-curve might struggle

M208 G28 G90 G94
M205 X150 Z80       ; Standard settings

G1 F1000 X50 Y50    ; Move to center

; Micro-moves (0.1mm steps)
G1 X50.1
G1 Y50.1
G1 X50.0
G1 Y50.0

; Small moves (1mm steps)
G1 X51
G1 Y51
G1 X50
G1 Y50

; Very fast micro-moves
G1 F5000
G1 X50.1 Y50.1
G1 X50.0 Y50.0
G1 X50.1 Y50.1
G1 X50.0 Y50.0

G28
```

**Expected behavior:**
- Smooth execution without jerky motion
- No stalling or hesitation
- Proper handling of very small segments

### Test 3B: Direction Changes Test

Save as `test_3b_directions.gcode`:
```gcode
; Test 3B: Sharp Direction Changes
; Tests rapid direction reversals

M208 G28 G90 G94
M205 X150 Z80

G1 F2000 X50 Y50

; Rapid reversals
G1 X60
G1 X40
G1 X60
G1 X40
G1 X50

; 90-degree corners
G1 Y60
G1 X60
G1 Y40
G1 X40
G1 Y50

; Acute angles
G1 X60 Y60
G1 X40 Y60
G1 X60 Y40
G1 X40 Y40
G1 X50 Y50

G28
```

**What to observe:**
- Smooth transitions at corners
- No overshoot or undershoot
- Consistent motion quality

### Test 3C: High-Speed Stress Test

Save as `test_3c_stress.gcode`:
```gcode
; Test 3C: High-Speed Stress Test
; Pushes S-curve to limits

M208 G28 G90 G94

; Progressive speed increase
G1 F1000 X10 Y10

; Speed test 1: 3000 mm/min
G1 F3000
G1 X90 Y10
G1 X90 Y90
G1 X10 Y90
G1 X10 Y10

; Speed test 2: 6000 mm/min
G1 F6000
G1 X90 Y10
G1 X90 Y90
G1 X10 Y90
G1 X10 Y10

; Speed test 3: 9000 mm/min (if machine capable)
G1 F9000
G1 X90 Y10
G1 X90 Y90
G1 X10 Y90
G1 X10 Y10

; Complex pattern at high speed
G1 F6000
G1 X50 Y10
G1 X90 Y30
G1 X70 Y50
G1 X90 Y70
G1 X50 Y90
G1 X10 Y70
G1 X30 Y50
G1 X10 Y30
G1 X50 Y10

G28
```

**Monitor for:**
- Missed steps or position loss
- Excessive vibration
- Controller errors or warnings

## Test 4: Adaptive Jerk Testing

**Purpose:** Validate adaptive jerk functionality

**Duration:** 10 minutes

### Test 4A: Adaptive Behavior Test

Save as `test_4a_adaptive.gcode`:
```gcode
; Test 4A: Adaptive Jerk Behavior
; Tests adaptive jerk response to different move types

M208 G28 G90 G94
M205 X150 Z80
M206 A1             ; Enable adaptive jerk

G1 F2000 X10 Y10

; Long moves (should use higher jerk)
G1 X90 Y10          ; 80mm move
G4 P0.5
G1 Y90              ; 80mm move
G4 P0.5

; Short moves (should use lower jerk)
G1 X85 Y85          ; 7mm move
G1 X80 Y90          ; 7mm move
G1 X85 Y85          ; 7mm move

; Mixed pattern
G1 X10 Y50          ; Long move
G1 X15 Y55          ; Short move
G1 X90 Y55          ; Long move
G1 X85 Y50          ; Short move

; Sharp corners (should reduce jerk)
G1 X90 Y10
G1 X10 Y10          ; 180-degree reversal
G1 X10 Y90
G1 X90 Y90          ; 90-degree corner

; Test without adaptive
M206 A0
G1 X50 Y10
G1 X10 Y10          ; Same moves without adaptive
G1 X10 Y50
G1 X50 Y50

G28
```

**Expected behavior:**
- Smoother motion with adaptive enabled
- Automatic adjustment based on move characteristics
- Better corner handling

## Test 5: Application-Specific Tests

Choose the test relevant to your application:

### Test 5A: 3D Printing Test

Save as `test_5a_printing.gcode`:
```gcode
; Test 5A: 3D Printing Test Pattern
; Simulates typical 3D printing moves

M208 G28 G90 G94
; 3D printing settings
M204 P3000 R5000 T1500
M205 X20 Z8 E40
M206 M1.0 C0.7 A1

; Simulate print moves
G1 F1800 X10 Y10    ; Travel move
; Perimeter simulation
G1 F1200 X90 Y10    ; Outer perimeter
G1 Y90
G1 X10
G1 Y10

; Inner perimeter
G1 X20 Y20
G1 X80 Y20
G1 Y80
G1 X20
G1 Y20

; Infill simulation (diagonal lines)
G1 F2400 X25 Y25
G1 X75 Y75
G1 X25 Y75
G1 X75 Y25
G1 X25 Y50
G1 X75 Y50

; Small details
G1 F600 X40 Y40
G1 X45 Y40
G1 X45 Y45
G1 X40 Y45
G1 X40 Y40

G28
```

### Test 5B: CNC Machining Test

Save as `test_5b_cnc.gcode`:
```gcode
; Test 5B: CNC Machining Test Pattern
; Simulates typical machining operations

M208 G28 G90 G94
; CNC settings
M204 P4000 T2000
M205 X25 Z12
M206 M1.2 C0.8 A1

; Roughing pass simulation
G1 F1500 Z-1        ; Plunge
G1 F2400 X10 Y10    ; Roughing moves
G1 X90
G1 Y15
G1 X10
G1 Y20
G1 X90
G1 Y25
G1 X10

; Finishing pass
G1 F1800 X10 Y10    ; Slower, more precise
G1 X90 Y10
G1 Y30
G1 X10
G1 Y10

; Profile cutting
G1 X50 Y15
G1 X85 Y25          ; Angled cut
G1 Y35
G1 X15
G1 Y25
G1 X50 Y15

; Pocket clearing
G1 X30 Y40
G1 X70 Y40
G1 Y70
G1 X30
G1 Y40

G1 Z1               ; Retract
G28
```

### Test 5C: Laser Cutting Test

Save as `test_5c_laser.gcode`:
```gcode
; Test 5C: Laser Cutting Test Pattern
; Simulates laser cutting operations

M208 G28 G90 G94
; Laser settings
M204 P6000 T3000
M205 X40 Z25
M206 M1.4 C0.8 A1

; Outside cut (fast)
G1 F3600 X10 Y10
G1 X90 Y10
G1 Y90
G1 X10
G1 Y10

; Detail cuts (slower)
G1 F1800 X30 Y30
G1 X70 Y30
G1 Y70
G1 X30
G1 Y30

; Intricate pattern
G1 F1200 X50 Y30
G1 X60 Y40
G1 X50 Y50
G1 X40 Y40
G1 X50 Y30

; Text simulation (small details)
G1 F900 X20 Y60
G1 Y80
G1 X25
G1 Y75
G1 X30 Y80
G1 Y60

G28
```

## Test Results Analysis

### Performance Metrics

For each test, record:

1. **Timing Data**
   - Total time to complete
   - Time per segment
   - Acceleration/deceleration phases

2. **Quality Measurements**
   - Corner accuracy (measure actual vs programmed)
   - Surface roughness (if measurable)
   - Position accuracy after test

3. **Behavior Observations**
   - Vibration levels
   - Noise characteristics
   - Any missed steps or errors

### Test Results Template

```
Test: [Test Name]
Date: [Date]
Settings: Jerk XY=[value] Z=[value] Multiplier=[value]

Results:
- Time: [seconds]
- Corner accuracy: [measurement]
- Surface quality: [rating 1-10]
- Vibration: [rating 1-10]
- Issues: [list any problems]

Recommended changes: [what to adjust]
```

## Tuning Procedure

Based on test results, follow this systematic tuning process:

### Step 1: Baseline Performance
1. Run Test 1A to verify S-curve is working
2. Run Test 2A with default settings to establish baseline
3. Record timing and quality metrics

### Step 2: Find Speed Limits
1. Gradually increase jerk values in 25% increments
2. Run Test 3C after each increase
3. Stop when vibration becomes excessive or steps are missed
4. Back off 20% from the limit

### Step 3: Optimize for Quality
1. Run Test 2B with the jerk settings from Step 2
2. If quality is poor, reduce jerk by 10-20%
3. Adjust corner factor (C parameter) to improve corners
4. Test with Test 3B

### Step 4: Enable Adaptive Features
1. Enable adaptive jerk: `M206 A1`
2. Run Test 4A to verify adaptive behavior
3. Fine-tune corner factor and multiplier

### Step 5: Application Testing
1. Run the appropriate Test 5 for your application
2. Make final adjustments based on real-world performance
3. Document final settings

### Step 6: Validation
1. Run complete test suite with final settings
2. Verify all tests pass acceptable criteria
3. Run longer test prints/jobs to confirm stability

## Troubleshooting Guide

### Common Test Failures

**Test 1A shows no difference:**
- S-curve may not be enabled
- Jerk values too similar to linear
- Check configuration settings

**Test 2A timing inconsistent:**
- Mechanical issues (loose belts, binding)
- Electrical interference
- Check hardware

**Test 3A micro-moves jerky:**
- Jerk values too high for small moves
- Reduce jerk or enable adaptive mode
- Check step resolution

**Test 3C causes missed steps:**
- Jerk or acceleration too high
- Mechanical limits exceeded
- Reduce all motion parameters

**Test 4A adaptive not working:**
- Feature not enabled in configuration
- Parameter out of range
- Check M206 A1 command

### Recovery Procedures

If tests cause problems:

1. **Immediate stop:** Send emergency stop or cycle power
2. **Reset parameters:** `M208` to return to defaults
3. **Check position:** `G28` to rehome if needed
4. **Restart testing:** Begin with very conservative settings

## Advanced Testing

### Oscilloscope Analysis

For advanced users with oscilloscope access:

1. **Monitor step signals** during Test 2A
2. **Analyze acceleration profiles** - should show smooth S-curves
3. **Check timing precision** - verify consistent step timing
4. **Measure jerk limitation** - validate theoretical vs actual

### Accelerometer Testing

If accelerometer available:

1. **Mount accelerometer** on machine axis
2. **Record during Test 3C** at various speeds
3. **Analyze frequency spectrum** - should show reduced high-frequency content
4. **Compare S-curve vs linear** motion profiles

## Final Validation

Before putting S-curve into production:

1. **Complete test suite passed** ✓
2. **Settings documented** ✓
3. **Performance meets requirements** ✓
4. **No mechanical issues observed** ✓
5. **Long-term test completed** (minimum 1 hour continuous operation) ✓

## Conclusion

This testing guide provides comprehensive validation of S-curve acceleration implementation. Follow the procedures systematically, document results carefully, and tune gradually for optimal performance.

The test G-code files are designed to stress different aspects of the S-curve system and validate proper operation across a wide range of scenarios. Regular testing with these patterns will help maintain optimal performance and catch any degradation over time.