# S-Curve Acceleration User Guide for CNC Machines

This guide provides comprehensive instructions for setting up and using the 7-phase S-curve acceleration system in grblHAL on higher-end DIY CNC machines. Designed for machines with NEMA23/NEMA24 stepper motors or servo systems like JMC 180W DC servos and 400W Stepperonline A6 series.

## Quick Start

### 1. Initial Setup

First, ensure S-curve acceleration is enabled in your configuration:

```c
#define ENABLE_S_CURVE_ACCELERATION On
#define S_CURVE_ENABLE_MCODES On
```

### 2. CNC Machine Parameter Overview

The S-curve system controls three critical aspects of CNC motion:

- **Jerk Values**: Control acceleration change rate (critical for surface finish and tool life)
- **Acceleration Values**: Maximum acceleration per axis (must match motor capabilities)
- **Advanced Parameters**: Junction optimization, path blending, and final deceleration control

### 3. Recommended Starting Values for Higher-End CNC Machines

**NEMA23/24 Stepper Motors:**
- XY Jerk: 300-500 mm/sec³ (higher torque allows more aggressive settings)
- Z Jerk: 150-250 mm/sec³ (vertical axis typically more conservative)
- Global Multiplier: 1.2 (take advantage of higher motor torque)

**Servo Motors (JMC 180W/400W A6 series):**
- XY Jerk: 500-800 mm/sec³ (servo feedback allows very aggressive settings)
- Z Jerk: 250-400 mm/sec³
- Global Multiplier: 1.5 (servo precision and torque advantages)

## Setting Parameters

### Method 1: Console Settings (Persistent)

These settings are saved permanently and survive reboots. All S-curve parameters are available as $ settings:

**Basic Jerk Settings:**
```
$800=400.0    ; X-axis jerk (mm/sec³) - NEMA23/24 capable
$801=400.0    ; Y-axis jerk (mm/sec³)  
$802=200.0    ; Z-axis jerk (mm/sec³) - more conservative for vertical
$803=300.0    ; A-axis jerk (mm/sec³) - rotary axis
$804=300.0    ; B-axis jerk (mm/sec³)
$805=300.0    ; C-axis jerk (mm/sec³)
```

**S-Curve Advanced Settings ($772-$786):**
```
$772=1.2      ; S-curve multiplier (global jerk scaling)
$773=0.8      ; Corner factor (jerk reduction in corners)
$774=1        ; Adaptive jerk enable (0=off, 1=on)
$775=1.3      ; Junction velocity factor
$776=1.2      ; Junction jerk multiplier
$777=150.0    ; Junction angle threshold (degrees)
$778=1        ; Path blending enable (0=off, 1=on)
$779=0.02     ; Path blending tolerance (mm)
$780=3.0      ; Path blending max radius (mm)
$781=50.0     ; Path blending min velocity (mm/min)
$782=0.7      ; Path blending jerk factor
$783=8        ; Path blending lookahead blocks (3-16)
$784=40.0     ; Min stop velocity for final deceleration (mm/min)
$785=2.5      ; Final deceleration jerk multiplier
$786=1.5      ; Stop threshold distance (mm)
```

**How to use:**
1. Connect to your controller via terminal
2. Type the setting command (e.g., `$800=400`)
3. Press Enter
4. Type `$$` to verify the setting was saved

### Method 2: M-Codes (Runtime Changes)

These settings apply immediately but reset on power cycle:

#### M204 - Set Acceleration
```gcode
M204 P8000 T4000     ; P=XY acceleration, T=Z acceleration (higher for NEMA23/24)
```

#### M205 - Set Jerk Parameters
```gcode
M205 X400 Z200       ; X=XY jerk, Z=Z jerk (aggressive for higher-end machines)
M205 J0.01           ; J=Junction deviation (affects cornering precision)
```

#### M206 - Advanced S-Curve Parameters
```gcode
M206 P1.3            ; P=Global jerk multiplier (0.5-2.0, higher for servos)
M206 Q0.7            ; Q=Corner jerk factor (0.1-1.0, lower = smoother corners)
M206 S1              ; S=Enable adaptive jerk (0=off, 1=on)
```

#### M207 - Report Current Settings
```gcode
M207                 ; Shows all current S-curve parameters
```

#### M208 - Reset to Defaults
```gcode
M208                 ; Resets all parameters to compiled defaults
```

#### M209 - Profile Presets
```gcode
M209 S0              ; Disable adaptive jerk
M209 S1              ; Enable adaptive jerk
M209 S2 V0.6         ; Set corner factor to 0.6
M209 S3 V1.4         ; Set global multiplier to 1.4 (aggressive for servos)
```

#### M210 - Junction Velocity Optimization
```gcode
M210 F1.4 J1.2 A120  ; F=velocity factor, J=jerk multiplier, A=angle threshold
```

#### M211 - Path Blending Configuration
```gcode
M211 S1 P0.02 R3.0 V50 F0.7 L8  ; Enable path blending with CNC-optimized settings
```

#### M212 - Final Deceleration Control
```gcode
M212 V40.0 Q2.5 D1.5 ; V=min stop velocity, Q=jerk multiplier, D=threshold distance
```

## CNC Machining Applications

### Material-Specific Settings

#### Aluminum Machining (Aggressive Cutting)
```gcode
; High-speed aluminum roughing
M204 P10000 T5000    ; Aggressive acceleration for rapid positioning
M205 X500 Z250       ; High jerk for responsive motion
M206 P1.4 Q0.8 S1    ; Aggressive multiplier with adaptive jerk

; Junction optimization for aluminum
M210 F1.5 J1.3 A90   ; Optimized for sharp corners in aluminum
M211 S1 P0.01 R2.5   ; Tight path blending for precision
```

#### Steel Machining (Precision Cutting)
```gcode
; Steel precision machining
M204 P6000 T3000     ; Moderate acceleration to reduce tool chatter
M205 X300 Z150       ; Conservative jerk for tool life
M206 P1.1 Q0.6 S1    ; Smooth motion with corner care

; Junction optimization for steel
M210 F1.2 J1.0 A120  ; Gentle corners to prevent tool breakage
M211 S1 P0.015 R2.0  ; Moderate blending for surface finish
```

#### Cast Iron Machining
```gcode
; Cast iron machining settings
M204 P4000 T2000     ; Lower acceleration for chatter control
M205 X200 Z100       ; Very conservative jerk
M206 P0.9 Q0.5 S1    ; Smooth motion priority

; Optimized for cast iron characteristics
M210 F1.0 J0.8 A150  ; Very gentle junction handling
M211 S1 P0.02 R1.5   ; Conservative blending for consistency
```

#### Composites and Plastics
```gcode
; Composite/plastic machining
M204 P8000 T4000     ; Higher speeds possible with these materials
M205 X400 Z200       ; Moderate jerk to prevent delamination
M206 P1.2 Q0.7 S1    ; Balanced approach

; Optimized for fiber composites
M210 F1.3 J1.1 A100  ; Smooth direction changes
M211 S1 P0.01 R3.0   ; Larger blend radius for fiber orientation
```

### Tool-Specific Optimizations

#### End Mill Operations
```gcode
; Standard end mill settings
M205 X350 Z180       ; Balanced for cutting forces
M206 P1.2 Q0.7       ; Good balance of speed and finish
M212 V45.0 Q2.3 D1.2 ; Smooth final approach for surface finish
```

#### Drilling Operations
```gcode
; Drilling cycle optimization
M205 X250 Z400       ; Lower XY jerk, higher Z for pecking
M206 P1.0 Q0.8       ; Conservative for hole accuracy
M212 V20.0 Q3.0 D0.5 ; Aggressive final approach for drilling
```

#### Fly Cutting and Large Cutters
```gcode
; Fly cutter settings (large interrupted cuts)
M205 X150 Z80        ; Very conservative for large cutter stability
M206 P0.8 Q0.4       ; Smooth motion critical for surface finish
M212 V30.0 Q1.8 D2.0 ; Gentle final approach
```

#### High-Speed Finishing
```gcode
; High-speed finishing passes
M205 X600 Z300       ; Aggressive for rapid traversals
M206 P1.5 Q0.9       ; Maximum responsiveness
M212 V60.0 Q2.8 D1.0 ; Quick final positioning
```

## Understanding Parameters

## Understanding Parameters for CNC Applications

### Jerk Values (Most Critical for CNC)

**What it does:** Controls how quickly acceleration can change, directly affecting:
- Surface finish quality (Ra values)
- Tool life and cutting forces
- Machine vibration and chatter
- Corner accuracy and dimensional precision

**CNC Machine Ranges:**
- **Heavy machines (1000+ lb):** 100-300 mm/sec³
- **Medium machines (300-1000 lb):** 250-500 mm/sec³  
- **Light machines (<300 lb):** 400-800 mm/sec³
- **Servo systems:** Add 50-100% to stepper values

**Material Impact:**
- **Soft materials (aluminum, plastics):** Can handle higher jerk values
- **Hard materials (steel, titanium):** Require lower jerk to prevent tool chatter
- **Interrupted cuts:** May need 30-50% reduction in jerk values

### Motor System Considerations

#### NEMA23/24 Stepper Motors
**Advantages:**
- Higher holding torque allows more aggressive acceleration
- Better performance at higher speeds
- Can maintain torque through S-curve acceleration phases

**Recommended Settings:**
```gcode
M205 X400 Z200       ; Base jerk values
M206 P1.2            ; 20% increase over NEMA17 settings
M210 F1.3 J1.2       ; Moderate junction optimization
```

#### Servo Motors (JMC 180W/400W A6 series)
**Advantages:**
- Closed-loop feedback allows very aggressive settings
- No missed steps - can push acceleration limits
- Superior performance in S-curve transition phases
- Better holding power during complex 7-phase profiles

**Servo-Optimized Settings:**
```gcode
M205 X600 Z350       ; Aggressive jerk values possible
M206 P1.5 Q0.8 S1    ; Take advantage of servo precision
M210 F1.5 J1.4 A90   ; Aggressive junction optimization
M211 S1 P0.01 R3.0 L12  ; Advanced path blending
```

**Servo Tuning Notes:**
- Start with 50% higher jerk values than equivalent steppers
- Monitor actual vs commanded position during tuning
- Use servo feedback to validate S-curve profile accuracy
- Enable advanced features like multi-block lookahead

### Acceleration Values for Higher-End Machines

**Typical CNC Acceleration Ranges:**
- **X/Y axes (NEMA23):** 5000-12000 mm/sec²
- **X/Y axes (400W servo):** 8000-20000 mm/sec²
- **Z axis (any motor):** 2000-8000 mm/sec² (depends on counterbalance)

**Rule of thumb:** Jerk should be 3-8% of acceleration value
- If acceleration = 10000 mm/sec², jerk should be 300-800 mm/sec³

### Advanced S-Curve Parameters Explained

#### Global Jerk Multiplier ($772, M parameter)
- **Range:** 0.5-2.0
- **CNC Typical:** 1.1-1.5
- **Effect:** Scales all jerk values simultaneously
- **1.0** = Standard setting
- **0.7-0.9** = Conservative (heavy machines, hard materials)
- **1.2-1.6** = Aggressive (servo systems, soft materials)

#### Corner Factor ($773, C parameter)
- **Range:** 0.1-1.0
- **CNC Typical:** 0.6-0.8
- **Effect:** Reduces jerk in corners to improve accuracy
- **1.0** = No corner reduction (sharp corners, potential overshoot)
- **0.5-0.8** = Moderate corner care (balanced precision/speed)
- **0.1-0.4** = Very careful corners (maximum precision, slower)

#### Adaptive Jerk Enable ($774, A parameter)
- **Values:** 0 (off) or 1 (on)
- **CNC Recommendation:** ON for most applications
- **Effect:** Automatically adjusts jerk based on move characteristics
- **Benefits:** 
  - Longer moves use higher jerk (faster rapids)
  - Short moves use lower jerk (better precision)
  - Automatic corner detection and jerk reduction

#### Junction Velocity Optimization ($775-$777)
**Velocity Factor ($775):** 0.5-2.0, typical 1.2-1.4 for CNC
- Controls how aggressively the system optimizes junction speeds
- Higher values = faster cornering, potential accuracy loss

**Jerk Multiplier ($776):** 0.5-2.0, typical 1.0-1.3 for CNC  
- Additional jerk scaling specifically for junction transitions
- Servo systems can handle 1.3-1.5

**Angle Threshold ($777):** 1-180 degrees, typical 90-150 for CNC
- Angles sharper than this threshold get special junction treatment
- Tighter angles (90°) = more conservative corner handling

#### Path Blending Parameters ($778-$783)
**Enable Path Blending ($778):** Recommended ON for CNC
- Smooths transitions between move segments
- Critical for complex 3D profiling and curved surfaces

**Tolerance ($779):** 0.001-1.0mm, typical 0.01-0.02mm for CNC
- Maximum deviation from programmed path during blending
- Tighter tolerance = more accurate, potentially slower

**Max Blend Radius ($780):** 0.1-100mm, typical 2-5mm for CNC
- Largest radius allowed for path blending
- Larger radius = smoother motion, more deviation from corners

**Min Velocity ($781):** 1-10000 mm/min, typical 30-100 for CNC
- Minimum speed before path blending activates
- Prevents blending during slow, precise operations

**Jerk Factor ($782):** 0.1-1.0, typical 0.6-0.8 for CNC
- Reduces jerk during blended segments
- Lower values = smoother blended paths

**Lookahead Blocks ($783):** 3-16, typical 6-12 for CNC
- Number of move blocks analyzed for optimization
- More blocks = better optimization, higher CPU usage

#### Final Deceleration Parameters ($784-$786)
**Min Stop Velocity ($784):** 0.1-1000 mm/min, typical 30-60 for CNC
- Switches to linear deceleration below this speed
- Prevents "crawling to stop" in jog motions

**Final Jerk Multiplier ($785):** 0.1-5.0, typical 2.0-3.0 for CNC
- Increases jerk for final approach to target
- Higher values = crisper stops, potential overshoot

**Stop Threshold Distance ($786):** 0.0-50mm, typical 1-3mm for CNC
- Distance from target where enhanced stopping activates
- Larger threshold = earlier aggressive deceleration

## CNC Machine Tuning Process

### Step 1: Machine Assessment
Before tuning, evaluate your machine characteristics:

**Motor Assessment:**
```gcode
; Test basic motor capabilities
M208                 ; Reset to defaults
M204 P5000 T2500     ; Conservative starting acceleration
M205 X200 Z100       ; Conservative starting jerk
```

**Mechanical Assessment:**
- Check all axes for binding, play, or irregular motion
- Verify belt tension and backlash compensation
- Ensure proper motor current settings
- Test maximum safe feed rates manually

### Step 2: Baseline S-Curve Setup

**NEMA23/24 Stepper Starting Point:**
```gcode
M208                 ; Reset to defaults
M205 X350 Z180       ; Moderate jerk for NEMA23/24
M206 P1.2 Q0.7 S1    ; Enable adaptive with moderate settings
M210 F1.3 J1.1 A120  ; Basic junction optimization
M211 S1 P0.02 R2.5 V50 F0.7 L8  ; Enable path blending
M212 V40.0 Q2.5 D1.5 ; Final deceleration optimization
```

**Servo Motor Starting Point:**
```gcode
M208                 ; Reset to defaults  
M205 X500 Z250       ; Aggressive jerk for servos
M206 P1.4 Q0.8 S1    ; Take advantage of servo precision
M210 F1.4 J1.3 A100  ; Aggressive junction optimization
M211 S1 P0.015 R3.0 V75 F0.8 L10  ; Advanced path blending
M212 V50.0 Q3.0 D1.8 ; Aggressive final deceleration
```

### Step 3: Progressive Jerk Increase

1. **Start with conservative values and test basic motion**
2. **Increase jerk by 25% increments until issues appear**
3. **Back off 20% from problem threshold**

**Test sequence:**
```gcode
; Test 1: Basic square motion
G90 G1 F2000 X50 Y50
G1 X0 Y0
; Observe: smooth motion, no vibration

; Test 2: Increase jerk 25%
M205 X437 Z225       ; 25% increase
; Repeat test, check for vibration/chatter

; Test 3: Continue until limit found
M205 X546 Z281       ; 50% increase from baseline
; Continue until vibration or poor surface finish appears
```

### Step 4: Material-Specific Optimization

**Aluminum Test Pattern:**
```gcode
; High-speed aluminum test
M205 X500 Z250       ; Aggressive settings
M206 P1.4 Q0.8       ; Fast cornering
; Run complex geometry at high feed rates
; Check surface finish and dimensional accuracy
```

**Steel Test Pattern:**
```gcode
; Steel precision test  
M205 X300 Z150       ; Conservative settings
M206 P1.1 Q0.6       ; Careful cornering
; Run same geometry, check for chatter
; Validate tool life and surface finish
```

### Step 5: Advanced Feature Tuning

**Junction Optimization:**
```gcode
; Test different junction settings
M210 F1.2 J1.0 A150  ; Conservative
; Run complex path with many direction changes
M210 F1.5 J1.3 A90   ; Aggressive  
; Compare cycle times and accuracy
```

**Path Blending:**
```gcode
; Test blending settings
M211 S1 P0.03 R2.0   ; Loose tolerance
; Run curved/complex geometry
M211 S1 P0.01 R3.0   ; Tight tolerance
; Compare surface finish vs cycle time
```

### Step 6: Final Validation

**Production Test:**
- Run actual parts with optimized settings
- Measure surface finish (Ra values)
- Check dimensional accuracy
- Monitor tool wear rates
- Validate cycle time improvements

**Save Final Settings:**
```
; Save optimized settings permanently
$772=1.3             ; Final multiplier
$773=0.7             ; Final corner factor  
$774=1               ; Adaptive enabled
$775=1.4             ; Junction velocity factor
$776=1.2             ; Junction jerk multiplier
$777=120             ; Junction angle threshold
$778=1               ; Path blending enabled
$779=0.015           ; Blend tolerance
$780=2.8             ; Max blend radius
$781=60              ; Min blend velocity
$782=0.7             ; Blend jerk factor
$783=10              ; Lookahead blocks
$784=45              ; Min stop velocity
$785=2.7             ; Final decel jerk multiplier
$786=1.6             ; Stop threshold distance
```

## Common CNC Issues and Solutions

### Issue: Poor Surface Finish

**Symptoms:** Visible tool marks, rough surfaces, poor Ra values
**Root Causes:** Jerk too high, inadequate corner handling, tool chatter
**Solutions:**
1. Reduce jerk values by 20-30%: `M205 X280 Z140`
2. Increase corner care: `M206 Q0.5`
3. Enable adaptive jerk if not already: `M206 S1`
4. Reduce junction aggressiveness: `M210 F1.0 J0.9`
5. Check mechanical issues (spindle runout, tool condition)

### Issue: Slow Cycle Times

**Symptoms:** Machine moves too conservatively, long cycle times
**Root Causes:** Jerk values too low, path blending disabled, conservative settings
**Solutions:**
1. Increase jerk incrementally: `M205 X400 Z200`
2. Increase global multiplier: `M206 P1.3`
3. Enable path blending: `M211 S1 P0.02 R3.0`
4. Optimize junctions: `M210 F1.4 J1.2 A100`
5. For servo systems, be more aggressive: `M206 P1.5`

### Issue: Tool Chatter or Vibration

**Symptoms:** Audible chatter, poor surface finish, premature tool wear
**Root Causes:** Resonance frequency, excessive jerk, mechanical issues
**Solutions:**
1. Reduce all jerk values by 40-50%
2. Lower corner factor: `M206 C0.4`
3. Disable path blending temporarily: `M211 S0`
4. Check mechanical condition (worn bearings, loose components)
5. Consider different spindle speeds to avoid resonance

### Issue: Dimensional Inaccuracy

**Symptoms:** Parts out of tolerance, corner rounding, overshoot
**Root Causes:** Aggressive cornering, path blending tolerance too loose
**Solutions:**
1. Tighten path blending tolerance: `M211 P0.005`
2. Reduce corner factor: `M206 C0.5`
3. Lower junction velocity factor: `M210 F1.0`
4. Enable adaptive jerk for automatic corner detection: `M206 A1`
5. Increase final deceleration precision: `M212 V20.0 Q1.8`

### Issue: Jog Motions "Crawl" to Stop

**Symptoms:** During jogging, machine slows excessively in final 1-2mm
**Root Causes:** Symmetric S-curve deceleration not optimized for stopping
**Solutions:**
1. **Immediate fix:** `M212 Q3.5 V60.0 D2.0`
2. **For NEMA23/24:** `M212 V50.0 Q3.0 D1.5`
3. **For servos:** `M212 V70.0 Q3.5 D2.0`
4. **If still crawling:** Increase Q to 4.0-4.5
5. **Save permanently:** `$785=3.2` (final decel jerk multiplier)

### Issue: Servo Motor Underutilization

**Symptoms:** Servo system performing like steppers, not taking advantage of feedback
**Root Causes:** Conservative settings designed for open-loop steppers
**Solutions:**
1. Increase base jerk values by 50-80%: `M205 X600 Z350`
2. Aggressive multiplier: `M206 P1.5`
3. Advanced junction optimization: `M210 F1.5 J1.4 A80`
4. Maximum lookahead: `M211 L16`
5. Aggressive final deceleration: `M212 Q3.5 V75.0`

### Issue: Inconsistent Performance Across Materials

**Symptoms:** Settings work well for one material but poorly for another
**Root Causes:** Single setting set not optimal for all applications
**Solutions:**
1. Create material-specific parameter sets
2. Use adaptive jerk: `M206 A1` (automatically adjusts)
3. Develop macros for quick material changes:
   ```gcode
   ; Aluminum macro
   M205 X500 Z250
   M206 P1.4 Q0.8
   M210 F1.5 J1.3
   
   ; Steel macro  
   M205 X300 Z150
   M206 P1.1 Q0.6
   M210 F1.2 J1.0
   ```

### Recovery Procedures

If S-curve settings cause problems:

1. **Emergency stop:** Send feed hold or emergency stop
2. **Reset parameters:** `M208` to return to safe defaults
3. **Check position:** `G28` to rehome if needed  
4. **Restart conservatively:** Begin with very low jerk values
5. **Verify mechanical condition** before increasing settings again

### Performance Monitoring

**Key Metrics to Track:**
- Surface roughness (Ra values)
- Dimensional accuracy (measured vs programmed)
- Cycle time improvements
- Tool life and wear rates
- Vibration levels during cutting

**Measurement Tools:**
- Surface roughness gauge for Ra measurements
- CMM or precision measuring tools for accuracy
- Timer for cycle time comparison
- Accelerometer for vibration analysis (if available)

## Verification Commands

### Check Current Settings
```gcode
M207                 ; Shows all S-curve parameters
$$                   ; Shows all grbl settings including axis jerk ($800-$805)
$G                   ; Shows current G-code parser state
```

### Verify S-Curve Operation
```gcode
; Simple test pattern
G90 G1 F2000 X25 Y25 ; Move to start position
G1 X75 Y25           ; Test X-axis S-curve
G1 X75 Y75           ; Test Y-axis S-curve  
G1 X25 Y75           ; Test combined motion
G1 X25 Y25           ; Return with corner
G28                  ; Return home
```

### Advanced Verification Tests
```gcode
; Junction optimization test
M210 F1.0 J1.0 A180  ; Conservative junctions
G1 F3000 X50 Y0      ; Fast move
G1 X0 Y50            ; 90-degree corner
G1 X50 Y0            ; Test junction behavior

; Path blending test
M211 S1 P0.02 R2.0   ; Enable blending
G1 F2000 X10 Y10
G1 X40 Y30 X70 Y10   ; Complex path
G1 X10 Y10           ; Return - should be smooth
```

## CNC Safety Guidelines

### Pre-Operation Checks
1. **Mechanical verification:** Check all axes move freely
2. **Parameter validation:** Verify settings with `M207` and `$$`
3. **Test motion:** Run air cuts before machining
4. **Tool condition:** Ensure sharp tools for optimal S-curve performance
5. **Workholding:** Secure workpiece to handle S-curve accelerations

### During Operation
1. **Monitor first part:** Watch for vibration, chatter, or poor finish
2. **Check dimensions:** Verify first part meets tolerances
3. **Tool wear:** Monitor for premature wear indicating poor settings
4. **Surface finish:** Validate Ra values meet requirements
5. **Cycle time:** Compare to baseline for efficiency gains

### Emergency Procedures
If S-curve motion causes problems:
1. **Feed hold:** Press feed hold to pause motion
2. **Emergency stop:** Use emergency stop if immediate halt needed
3. **Reset parameters:** `M208` to return to safe defaults
4. **Rehome:** `G28` if position may be lost
5. **Mechanical check:** Inspect for damage before resuming

## Machine-Specific Recommendations

### FlexiHAL with STM32F446 Controller

**Optimal Settings for NEMA23 Steppers:**
```
$800=400.0    ; X-axis jerk
$801=400.0    ; Y-axis jerk
$802=200.0    ; Z-axis jerk
$772=1.2      ; S-curve multiplier
$773=0.7      ; Corner factor
$774=1        ; Adaptive jerk enable
$775=1.3      ; Junction velocity factor
$776=1.2      ; Junction jerk multiplier
$777=120      ; Junction angle threshold
$778=1        ; Path blending enable
$779=0.015    ; Path blending tolerance
$780=2.5      ; Path blending max radius
$781=50       ; Path blending min velocity
$782=0.7      ; Path blending jerk factor
$783=8        ; Path blending lookahead blocks
$784=40       ; Min stop velocity
$785=2.5      ; Final decel jerk multiplier
$786=1.5      ; Stop threshold distance
```

**For Servo Systems (400W A6 series):**
```
$800=600.0    ; X-axis jerk (50% higher)
$801=600.0    ; Y-axis jerk
$802=300.0    ; Z-axis jerk
$772=1.5      ; S-curve multiplier (more aggressive)
$773=0.8      ; Corner factor
$774=1        ; Adaptive jerk enable
$775=1.5      ; Junction velocity factor
$776=1.4      ; Junction jerk multiplier
$777=100      ; Junction angle threshold (tighter)
$778=1        ; Path blending enable
$779=0.01     ; Path blending tolerance (tighter)
$780=3.0      ; Path blending max radius
$781=75       ; Path blending min velocity
$782=0.8      ; Path blending jerk factor
$783=12       ; Path blending lookahead blocks (more)
$784=60       ; Min stop velocity
$785=3.0      ; Final decel jerk multiplier
$786=2.0      ; Stop threshold distance
```

## Performance Expectations

### Expected Improvements with Optimized S-Curve

**Surface Finish:**
- **15-25% improvement** in Ra values compared to linear acceleration
- **Smoother tool marks** and reduced chatter marks
- **Better corner quality** with reduced overshoot

**Cycle Time:**
- **10-20% faster** complex geometry machining
- **Higher feed rates** possible through intricate paths
- **Reduced dwell time** at direction changes

**Tool Life:**
- **20-30% longer** tool life due to reduced shock loads
- **Less chipping** on carbide tools from smoother acceleration
- **Better chip evacuation** with consistent motion profiles

**Machine Health:**
- **Reduced vibration** and structural stress
- **Less wear** on drive components (belts, screws, bearings)
- **Quieter operation** with smoother motion profiles

### Measurement and Validation

**Before/After Comparison:**
1. **Baseline measurement:** Record Ra, cycle time, and tool life with linear acceleration
2. **S-curve optimization:** Implement and tune S-curve settings
3. **Performance validation:** Measure improvements and document settings
4. **Production verification:** Run extended tests to confirm benefits

This user guide provides the foundation for implementing S-curve acceleration on higher-end CNC machines. For comprehensive testing and validation procedures, refer to the S-Curve Testing and Tuning Guide.