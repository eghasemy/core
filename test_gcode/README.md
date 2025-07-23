# S-Curve Acceleration Test G-Code Files for CNC Machines

This directory contains comprehensive test files for validating and optimizing S-curve acceleration on CNC machines with NEMA23/NEMA24 stepper motors or servo systems.

**Travel Envelope: Designed for 500x500x200mm machines with 25mm safety margins**

## Custom Travel Limits

If your machine has different travel limits, use the G-code generator:

```bash
# Generate test files for your specific machine
python ../generate_test_gcode.py [x_max] [y_max] [z_max]

# Examples:
python ../generate_test_gcode.py 300 300 150    # Smaller machine  
python ../generate_test_gcode.py 800 400 200    # Larger machine
python ../generate_test_gcode.py 1000 1000 300  # Large format machine
```

**Generator Features:**
- Automatically scales coordinates to your travel limits
- Maintains safety margins and proportional test patterns  
- Generates optimized test files for your specific envelope
- Creates configuration documentation
- Output files saved to `test_gcode_generated/` directory

## Test File Overview

### Basic Functionality Tests
- **test_1a_comparison.gcode** - Compares linear vs S-curve motion profiles for CNC
- **test_1b_parameters.gcode** - Verifies parameter changes take effect

### Performance and Accuracy Tests  
- **test_2a_speed.gcode** - Speed performance characterization
- **test_2b_surface.gcode** - Surface quality evaluation
- **test_3a_micromoves.gcode** - Micro-move handling validation
- **test_3b_directions.gcode** - Direction change and corner tests
- **test_3c_stress.gcode** - High-speed stress testing

### Advanced Feature Tests
- **test_4a_adaptive.gcode** - Adaptive jerk functionality validation
- **junction_optimization_test.gcode** - Junction velocity optimization
- **path_blending_test.gcode** - Path blending feature testing
- **lookahead_optimization_test.gcode** - Multi-block lookahead testing
- **test_final_deceleration.gcode** - Final deceleration optimization

### CNC Application-Specific Tests
- **test_5b_cnc.gcode** - General CNC machining operations
- **test_cnc_aluminum.gcode** - Aluminum machining optimization
- **test_cnc_steel.gcode** - Steel machining optimization  
- **test_servo_optimization.gcode** - Servo motor specific testing

## Motor System Compatibility

### NEMA23/24 Stepper Motors
Most test files are compatible with NEMA23/24 steppers using moderate to aggressive settings:
- Recommended jerk range: 250-500 mm/sec³
- Use multiplier values: 1.1-1.3
- Monitor for missed steps during high-speed tests

### Servo Motors (JMC 180W, 400W A6 series)
Servo systems can handle much more aggressive settings:
- Recommended jerk range: 500-800 mm/sec³  
- Use multiplier values: 1.3-1.6
- Monitor following errors and servo drive status

## Usage Instructions

### Pre-Test Setup
1. Ensure machine is properly homed and calibrated
2. Verify all mechanical components are secure
3. Set appropriate work coordinate system (G54)
4. Have emergency stop readily accessible

### Running Tests
1. Start with basic functionality tests (test_1a, test_1b)
2. Progress to performance tests (test_2a, test_2b)
3. Run application-specific tests for your material/motor type
4. Document results for optimization decisions

### Safety Considerations
- All test files use positioning moves only (no cutting)
- For cutting tests, user must configure appropriate:
  - Spindle speeds
  - Feed rates  
  - Depth of cut
  - Tool selection
- Monitor machine behavior and stop if vibration or errors occur

## Test Sequence Recommendation

### Phase 1: Basic Validation (10 minutes)
```gcode
M208    ; Reset S-curve parameters to defaults
G28     ; Home all axes
M207    ; Report current settings
```
1. **test_1a_comparison.gcode** - Verify S-curve vs linear differences
2. **test_1b_parameters.gcode** - Confirm parameter changes work

### Phase 2: Performance Baseline (20 minutes)
1. **test_2a_speed.gcode** - Establish speed performance baseline
2. **test_2b_surface.gcode** - Test surface quality effects

### Phase 3: Edge Case Validation (25 minutes)
1. **test_3a_micromoves.gcode** - Validate small move handling
2. **test_3b_directions.gcode** - Test corner accuracy and direction changes
3. **test_3c_stress.gcode** - Find performance limits

### Phase 4: Advanced Features (15 minutes)
1. **test_4a_adaptive.gcode** - Adaptive jerk functionality
2. **junction_optimization_test.gcode** - Junction optimization
3. **path_blending_test.gcode** - Path blending validation

### Phase 5: CNC Application Testing (30 minutes)
Choose based on your machine and materials:
- **test_cnc_aluminum.gcode** - For aluminum machining
- **test_cnc_steel.gcode** - For steel machining  
- **test_servo_optimization.gcode** - For servo motor systems
- **test_5b_cnc.gcode** - General CNC operations

### Phase 6: Final Optimization (10 minutes)
1. **test_final_deceleration.gcode** - Eliminate jog crawling issues

## Material-Specific Testing

### Aluminum Machining
- Use **test_cnc_aluminum.gcode** for aluminum-specific optimization
- Aluminum allows aggressive settings due to excellent machinability
- Focus on high-speed performance and cycle time reduction

### Steel Machining  
- Use **test_cnc_steel.gcode** for steel-specific optimization
- Steel requires conservative settings to prevent chatter
- Prioritize tool life and surface finish over speed

### General CNC Operations
- Use **test_5b_cnc.gcode** for balanced CNC testing
- Covers roughing, finishing, profiling, and pocketing operations
- Good starting point for mixed-material machining

## Test Results Documentation

Create a testing log documenting:
- Machine configuration (motor type, controller, mechanical specs)
- Test file results and observations
- Optimal parameter values discovered
- Any issues or failures encountered
- Final production settings chosen

## Interpreting Results

### Good Results
- Smooth, quiet motion without vibration
- Accurate positioning maintained throughout tests
- No missed steps or controller errors
- Improved surface quality vs linear motion
- Better corner accuracy and reduced overshoot

### Warning Signs
- Excessive vibration or unusual noise patterns
- Missed steps or position errors
- Jerky or inconsistent motion characteristics
- Poor surface finish or dimensional accuracy
- Controller errors or following errors (servos)

### Common Issues and Solutions
- **Motion too slow**: Increase jerk values gradually (25% increments)
- **Too aggressive**: Reduce jerk values by 20-30%
- **Poor corners**: Decrease corner factor (C parameter)
- **Inconsistent motion**: Enable adaptive jerk (A=1)
- **Jog crawling**: Optimize final deceleration parameters (M212)

## Advanced Feature Testing

### Junction Optimization
Run **junction_optimization_test.gcode** to optimize:
- Junction velocity factors ($775)
- Junction jerk multipliers ($776)
- Angle thresholds for junction treatment ($777)

### Path Blending
Run **path_blending_test.gcode** to optimize:
- Blending tolerance for accuracy vs speed ($779)
- Maximum blend radius settings ($780)
- Lookahead block configuration ($783)

### Final Deceleration
Run **test_final_deceleration.gcode** to eliminate:
- "Crawling to stop" in jog motions
- Slow final approach issues
- Optimize crisp positioning

## Safety Notes

- **Emergency Stop**: Have emergency stop readily available during all tests
- **Start Conservative**: Always begin with low jerk values and increase gradually
- **Monitor Closely**: Watch for signs of mechanical stress or unusual behavior
- **Test Incrementally**: Make small parameter changes between tests
- **Verify Position**: Home machine after any test that shows position errors

## Performance Expectations

### Typical Improvements with Optimized S-Curve
- **15-25% better surface finish** (Ra values)
- **10-20% faster cycle times** for complex geometry
- **20-30% longer tool life** due to reduced shock loads
- **Improved corner accuracy** and dimensional precision
- **Reduced machine vibration** and structural stress

## Support and Documentation

For detailed instructions and troubleshooting, see:
- **S_CURVE_USER_GUIDE.md** - Complete parameter setup guide for CNC machines
- **S_CURVE_TESTING_GUIDE.md** - Comprehensive testing procedures and analysis
- **S_CURVE_ACCELERATION.md** - Technical implementation details

## File Format and Compatibility

All files use standard G-code format compatible with grblHAL controllers. Files include:
- Parameter reset commands (M208)
- Homing commands (G28)  
- CNC-optimized S-curve settings
- Motion test patterns appropriate for CNC machines
- Return to safe position commands

Each file is self-contained and can be run independently, though following the recommended sequence provides the most comprehensive validation for CNC machining applications.