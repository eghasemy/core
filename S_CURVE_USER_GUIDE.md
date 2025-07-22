# S-Curve Acceleration User Guide

This guide provides step-by-step instructions for setting up and using the 7-phase S-curve acceleration system in grblHAL. Whether you're new to S-curve motion or an experienced user, this guide will help you get the best performance from your machine.

## Quick Start

### 1. Initial Setup

First, ensure S-curve acceleration is enabled in your configuration:

```c
#define ENABLE_S_CURVE_ACCELERATION On
#define S_CURVE_ENABLE_MCODES On
```

### 2. Basic Parameter Overview

The S-curve system has three main types of parameters:

- **Jerk Values**: Control motion smoothness (lower = smoother, higher = faster)
- **Acceleration Values**: Maximum acceleration for each axis
- **Advanced Parameters**: Fine-tuning for specific applications

### 3. Default Settings

Out of the box, the system uses these conservative defaults:
- XY Jerk: 150 mm/sec³
- Z Jerk: 80 mm/sec³  
- E Jerk: 120 mm/sec³
- Global Multiplier: 1.0

## Setting Parameters

### Method 1: Console Settings (Persistent)

These settings are saved permanently and survive reboots:

```
$800=150.0    ; X-axis jerk (mm/sec³)
$801=150.0    ; Y-axis jerk (mm/sec³)  
$802=80.0     ; Z-axis jerk (mm/sec³)
$803=120.0    ; A-axis jerk (mm/sec³)
$804=120.0    ; B-axis jerk (mm/sec³)
$805=120.0    ; C-axis jerk (mm/sec³)
```

**How to use:**
1. Connect to your controller via terminal
2. Type the setting command (e.g., `$800=200`)
3. Press Enter
4. Type `$$` to verify the setting was saved

### Method 2: M-Codes (Temporary)

These settings apply immediately but reset on power cycle:

#### M204 - Set Acceleration
```gcode
M204 P3000 T1500    ; P=XY acceleration, T=Z acceleration
M204 R2000          ; R=Retract/E-axis acceleration
```

#### M205 - Set Jerk Parameters
```gcode
M205 X20 Z10 E15    ; X=XY jerk, Z=Z jerk, E=E jerk
M205 J0.02          ; J=Junction deviation (affects cornering)
```

#### M206 - Advanced Parameters
```gcode
M206 M1.2           ; M=Global jerk multiplier (0.1-5.0)
M206 C0.8           ; C=Corner jerk factor (0.1-1.0)
M206 A1             ; A=Enable adaptive jerk (0=off, 1=on)
```

#### M207 - Report Current Settings
```gcode
M207                ; Shows all current S-curve parameters
```

#### M208 - Reset to Defaults
```gcode
M208                ; Resets all parameters to compiled defaults
```

#### M209 - Profile Presets
```gcode
M209 S0             ; Disable adaptive jerk
M209 S1             ; Enable adaptive jerk
M209 S2 V0.6        ; Set corner factor to 0.6
M209 S3 V1.3        ; Set global multiplier to 1.3
```

## Application-Specific Settings

### 3D Printing

**For PLA/PETG (typical settings):**
```gcode
; Basic acceleration
M204 P3000 R5000 T1500

; Jerk settings
M205 X20 Z8 E40

; Fine-tuning
M206 M1.0 C0.7 A1
```

**For flexible filaments:**
```gcode
M204 P1500 R2000 T800
M205 X10 Z5 E20
M206 M0.8 C0.6 A1
```

### CNC Machining

**For aluminum (aggressive cutting):**
```gcode
M204 P5000 T2500
M205 X30 Z15
M206 M1.3 C0.9 A1
```

**For steel (precision cutting):**
```gcode
M204 P2000 T1000
M205 X12 Z6
M206 M0.8 C0.6 A1
```

**For finishing passes:**
```gcode
M204 P1000 T500
M205 X8 Z4
M206 M0.7 C0.5 A1
```

### Laser Cutting

**For fast cutting:**
```gcode
M204 P8000 T4000
M205 X50 Z30
M206 M1.5 C0.8 A1
```

**For detailed engraving:**
```gcode
M204 P2000 T1000
M205 X15 Z10
M206 M0.9 C0.5 A1
```

## Understanding Parameters

### Jerk Values (Most Important)

**What it does:** Controls how quickly acceleration can change
- **Lower values** = Smoother motion, better surface finish, slower
- **Higher values** = More responsive motion, faster, potential vibration

**Typical ranges:**
- **Light machines (3D printers):** 100-300 mm/sec³
- **Medium machines (desktop CNC):** 50-150 mm/sec³  
- **Heavy machines (mill/lathe):** 20-80 mm/sec³

### Acceleration Values

**What it does:** Maximum acceleration for each axis
- Set via M204 or machine settings
- Should match your machine's capabilities
- Too high = missed steps, too low = slow motion

### Advanced Parameters

**Jerk Multiplier (M parameter):**
- 1.0 = Standard setting
- 0.5-0.9 = More conservative (smoother)
- 1.1-2.0 = More aggressive (faster)

**Corner Factor (C parameter):**
- 1.0 = No corner reduction
- 0.5-0.8 = Moderate corner care
- 0.1-0.4 = Very careful corners

**Adaptive Jerk (A parameter):**
- 0 = Fixed jerk values
- 1 = Automatic adjustment based on move characteristics

### Final Deceleration Settings (New!)

These settings address the "crawling to stop" issue in jog motions:

**M212 Command - Final Deceleration Optimization:**
```gcode
M212 V30 Q2.0 D1.0
```

**Parameters:**
- **V (Min Stop Velocity):** Minimum velocity for S-curve deceleration (mm/min)
  - Below this velocity, linear deceleration is used for faster stopping
  - Range: 0.1-1000.0 mm/min
  - Default: 30.0 mm/min

- **Q (Final Decel Jerk Multiplier):** Jerk multiplier for final deceleration phase
  - Higher values provide faster stopping
  - Range: 0.1-5.0
  - Default: 2.0

- **D (Stop Threshold Distance):** Distance from target to switch to rapid stopping (mm)
  - Set to 0.0 to disable threshold-based stopping
  - Range: 0.0-50.0 mm
  - Default: 1.0 mm

**Console Settings (Persistent):**
```
$784=30.0    ; Min stop velocity (mm/min)
$785=2.0     ; Final decel jerk multiplier
$786=1.0     ; Stop threshold distance (mm)
```

**When to Adjust:**
- If jog motions "crawl" to a stop: Increase Q parameter (try 3.0-4.0)
- If stopping is too abrupt: Decrease Q parameter (try 1.0-1.5)
- For very precise applications: Decrease V parameter (try 10-20 mm/min)
- For faster jogging: Increase V parameter (try 50-100 mm/min)

## Step-by-Step Tuning Process

### Step 1: Start Conservative
```gcode
M208                ; Reset to defaults
M205 X10 Z5         ; Start with low jerk
M206 M0.8 C0.6 A1   ; Conservative settings
```

### Step 2: Test Basic Motion
Run simple moves and observe:
- Are moves too slow? Increase jerk values
- Vibration or poor surface? Decrease jerk values
- Sharp corners poor? Decrease corner factor

### Step 3: Gradually Increase Performance
```gcode
M205 X15 Z8         ; Increase jerk by 50%
; Test again
M205 X20 Z10        ; Continue increasing until issues appear
; Then back off 20%
```

### Step 4: Fine-Tune for Application
```gcode
; For speed-critical applications
M206 M1.2 C0.8

; For quality-critical applications  
M206 M0.8 C0.5
```

## Common Issues and Solutions

### Issue: Motion is Too Slow

**Symptoms:** Machine crawls through complex paths
**Solutions:**
1. Increase jerk values: `M205 X25 Z12`
2. Increase multiplier: `M206 M1.3`
3. Check if adaptive is disabled: `M206 A1`

### Issue: Poor Surface Finish

**Symptoms:** Visible layer lines, rough surfaces
**Solutions:**
1. Decrease jerk values: `M205 X8 Z4`
2. Increase corner care: `M206 C0.4`
3. Lower multiplier: `M206 M0.7`

### Issue: Machine Vibration

**Symptoms:** Audible vibration, part movement
**Solutions:**
1. Reduce all jerk values by 30-50%
2. Lower corner factor: `M206 C0.3`
3. Check mechanical issues (loose belts, worn bearings)

### Issue: Poor Corner Accuracy

**Symptoms:** Rounded corners, overshoot
**Solutions:**
1. Enable adaptive jerk: `M206 A1`
2. Reduce corner factor: `M206 C0.5`
3. Lower junction deviation: `M205 J0.01`

### Issue: Inconsistent Motion

**Symptoms:** Jerky motion, varying speeds
**Solutions:**
1. Check acceleration settings with `M207`
2. Ensure jerk values are appropriate for accelerations
3. Verify mechanical condition

## Parameter Relationships

### Jerk vs Acceleration
- Jerk should typically be 5-20% of acceleration value
- If acceleration = 2000 mm/sec², jerk should be 100-400 mm/sec³

### Speed vs Quality Trade-offs
- **High Speed:** Higher jerk, higher corner factor, adaptive enabled
- **High Quality:** Lower jerk, lower corner factor, conservative multiplier

### Machine Weight Impact
- **Light machines:** Can handle higher jerk values
- **Heavy machines:** Need lower jerk values to prevent vibration

## Troubleshooting Common Issues

### Issue: Jog Motions "Crawl" to Stop

**Symptoms:** During jogging, the machine slows down excessively in the last 1-2mm and "crawls" to the final position.

**Solution 1 - Immediate Fix:**
```gcode
M212 Q3.0 V50.0 D2.0    ; Increase final decel aggressiveness
```

**Solution 2 - Fine Tuning:**
```gcode
; Start with defaults
M212 V30.0 Q2.0 D1.0

; If still crawling, increase Q progressively:
M212 Q2.5
M212 Q3.0
M212 Q3.5

; Or increase min stop velocity:
M212 V50.0
M212 V75.0
```

**Solution 3 - Console Settings (Permanent):**
```
$785=3.0     ; Higher final jerk multiplier
$784=50.0    ; Higher min stop velocity
$786=2.0     ; Larger stop threshold
```

**What this does:**
- Higher Q values make the final deceleration more aggressive
- Higher V values switch to linear deceleration sooner
- Higher D values trigger fast stopping earlier

### Issue: Too Abrupt Stopping

**Symptoms:** Jog motions stop too quickly, causing mechanical shock.

**Solution:**
```gcode
M212 Q1.0 V15.0 D0.5    ; More gentle final deceleration
```

### Issue: Inconsistent Jerk Settings

**Symptoms:** Different behavior at low vs high jerk settings.

**Check Settings:**
```gcode
M207                    ; View all current parameters
M212                    ; Check final decel settings
$$                      ; Check axis jerk values ($800-$805)
```

## Verification Commands

### Check Current Settings
```gcode
M207                ; Shows all S-curve parameters
$$                  ; Shows all grbl settings including jerk
```

### Verify Operation
```gcode
G1 F1000 X10 Y10    ; Simple move to test
G1 X0 Y0            ; Return move
```

## Safety Notes

1. **Always start conservative** - Begin with low jerk values and increase gradually
2. **Test thoroughly** - Run test patterns before production work
3. **Monitor for missed steps** - Reduce settings if you hear skipping
4. **Check mechanical condition** - Loose components amplify vibration issues

## Getting Help

If you encounter issues:

1. **Report current settings:** Use `M207` and `$$`
2. **Describe the problem:** Include symptoms and when they occur
3. **Share your setup:** Machine type, controller, application
4. **Try conservative settings first:** Use `M208` to reset to defaults

This user guide should get you started with S-curve acceleration. For advanced tuning and testing procedures, see the S-Curve Testing and Tuning Guide.