# S-Curve Test G-Code Files

This directory contains comprehensive test G-code files for validating S-curve acceleration performance. Each test is designed to evaluate specific aspects of the S-curve implementation.

## Test Files Overview

### Basic Functionality Tests
- **test_1a_comparison.gcode** - Compares S-curve vs linear motion
- **test_1b_parameters.gcode** - Tests parameter response and changes

### Performance Tests  
- **test_2a_speed.gcode** - Measures speed performance with complex paths
- **test_2b_surface.gcode** - Creates test patches for surface quality measurement

### Edge Case Tests
- **test_3a_micromoves.gcode** - Tests very small moves and micro-positioning
- **test_3b_directions.gcode** - Tests sharp direction changes and corners
- **test_3c_stress.gcode** - High-speed stress testing to find limits

### Feature Tests
- **test_4a_adaptive.gcode** - Tests adaptive jerk functionality

### Application-Specific Tests
- **test_5a_printing.gcode** - 3D printing simulation patterns
- **test_5b_cnc.gcode** - CNC machining operation simulation
- **test_5c_laser.gcode** - Laser cutting operation simulation

## Usage Instructions

1. **Safety First**: Ensure your machine is properly homed and safe to operate
2. **Start Conservative**: Begin with default S-curve settings
3. **Run Tests Sequentially**: Follow the numerical order for systematic validation
4. **Document Results**: Record timing, quality, and any issues observed
5. **Tune Gradually**: Make incremental adjustments based on test results

## Before Running Tests

```gcode
M208    ; Reset S-curve parameters to defaults
G28     ; Home all axes
M207    ; Report current settings
```

## Test Sequence Recommendation

1. **Basic Validation** (5 minutes)
   - test_1a_comparison.gcode
   - test_1b_parameters.gcode

2. **Performance Baseline** (10 minutes)
   - test_2a_speed.gcode
   - test_2b_surface.gcode

3. **Edge Case Validation** (15 minutes)
   - test_3a_micromoves.gcode
   - test_3b_directions.gcode
   - test_3c_stress.gcode

4. **Feature Testing** (5 minutes)
   - test_4a_adaptive.gcode

5. **Application Testing** (10 minutes)
   - Choose appropriate test_5x_*.gcode for your use case

## Interpreting Results

### Good Results
- Smooth, quiet motion
- Accurate positioning
- No missed steps or vibration
- Improved surface quality vs linear motion

### Warning Signs
- Excessive vibration or noise
- Missed steps or position errors
- Jerky or inconsistent motion
- Poor surface finish

### Common Issues
- **Too slow**: Increase jerk values gradually
- **Too aggressive**: Reduce jerk values by 20-30%
- **Poor corners**: Adjust corner factor (C parameter)
- **Inconsistent**: Enable adaptive jerk (A=1)

## Safety Notes

- **Emergency Stop**: Have emergency stop readily available
- **Start Conservative**: Always begin with low jerk values
- **Monitor Closely**: Watch for signs of mechanical stress
- **Test Incrementally**: Make small parameter changes between tests

## Support

For detailed instructions and troubleshooting, see:
- **S_CURVE_USER_GUIDE.md** - Step-by-step parameter setup
- **S_CURVE_TESTING_GUIDE.md** - Comprehensive testing procedures
- **S_CURVE_ACCELERATION.md** - Technical implementation details

## File Format

All files use standard G-code format compatible with grblHAL controllers. Files include:
- Parameter reset commands (M208)
- Homing commands (G28)  
- Test-specific S-curve settings
- Motion test patterns
- Return to safe position

Each file is self-contained and can be run independently, though following the recommended sequence provides the most comprehensive validation.