#!/usr/bin/env python3
"""
S-Curve Test G-Code Generator for CNC Machines
Generates test G-code files based on configurable travel limits.

Usage: python generate_test_gcode.py [x_max] [y_max] [z_max]
Default: 500 500 200 (mm)
"""

import os
import sys
import math

class GCodeGenerator:
    def __init__(self, x_max=500, y_max=500, z_max=200, safety_margin=25):
        self.x_max = x_max
        self.y_max = y_max  
        self.z_max = z_max
        self.safety_margin = safety_margin
        
        # Calculate usable ranges
        self.x_min = safety_margin
        self.x_usable = x_max - safety_margin
        self.y_min = safety_margin  
        self.y_usable = y_max - safety_margin
        self.z_min = 5  # Conservative Z minimum
        self.z_usable = min(z_max - 10, 100)  # Most tests don't need full Z
        
        # Calculate center points
        self.x_center = (self.x_min + self.x_usable) / 2
        self.y_center = (self.y_min + self.y_usable) / 2
        
        # Calculate test areas
        self.small_range = min(150, (self.x_usable - self.x_min) * 0.3)  # 30% of usable
        self.medium_range = min(300, (self.x_usable - self.x_min) * 0.6)  # 60% of usable  
        self.large_range = min(450, (self.x_usable - self.x_min) * 0.9)   # 90% of usable

    def header(self, title, description=""):
        """Generate standard G-code header"""
        return f"""; {title}
; {description}
; Generated for travel envelope: {self.x_max}x{self.y_max}x{self.z_max}mm
; Usable range: X{self.x_min}-{self.x_usable}, Y{self.y_min}-{self.y_usable}, Z{self.z_min}-{self.z_usable}

"""

    def generate_test_1a_comparison(self):
        """Basic S-curve vs linear comparison test"""
        # Use medium range for this test
        x_start = self.x_center - self.medium_range/2
        x_end = self.x_center + self.medium_range/2
        y_start = self.y_center - self.medium_range/2
        y_end = self.y_center + self.medium_range/2
        
        content = self.header("Test 1A: S-curve vs Linear Motion Comparison", 
                            "Verifies S-curve system is functioning correctly")
        
        content += f"""M208                ; Reset to defaults
G28                 ; Home all axes
G90 G94             ; Absolute, feed rate mode

; Test 1: Very low jerk (approximates linear)
M205 X25 Z15        ; Minimal jerk settings
G1 F1000 X{x_start:.0f} Y{y_start:.0f}  ; Move to start position
G4 P2               ; Pause for observation

; Execute test pattern with minimal S-curve - {self.medium_range:.0f}mm square
G1 F2500 X{x_end:.0f} Y{y_start:.0f}   ; Fast move in X ({self.medium_range:.0f}mm travel)
G4 P1
G1 Y{y_end:.0f}             ; Fast move in Y ({self.medium_range:.0f}mm travel)  
G4 P1
G1 X{x_start:.0f}             ; Return move ({self.medium_range:.0f}mm travel)
G4 P1
G1 Y{y_start:.0f}             ; Complete square ({self.medium_range:.0f}mm travel)
G4 P2

; Test 2: Proper S-curve settings
M205 X300 Z150      ; Proper S-curve jerk values
M206 P1.2 Q0.7      ; Enable full S-curve features
G4 P2               ; Pause for comparison

; Execute same pattern with full S-curve
G1 F2500 X{x_end:.0f} Y{y_start:.0f}   ; Same moves with S-curve enabled
G4 P1
G1 Y{y_end:.0f}
G4 P1  
G1 X{x_start:.0f}
G4 P1
G1 Y{y_start:.0f}
G4 P2

; Return to home
G28
M207                ; Report final settings

; Expected observations:
; - Test 1: More abrupt motion, potential vibration
; - Test 2: Smoother acceleration/deceleration, less vibration  
; - No missed steps or position errors throughout
; - Clear audible differences in motor sound characteristics
"""
        return content

    def generate_test_1b_parameters(self):
        """Parameter response verification test"""
        # Use center area for this test
        move_distance = min(150, self.small_range)
        x_start = self.x_center - move_distance/2
        x_end = self.x_center + move_distance/2
        y_start = self.y_center - move_distance/2
        y_end = self.y_center + move_distance/2
        
        content = self.header("Test 1B: Parameter Response Verification", 
                            "Tests immediate response to parameter modifications")
        
        content += f"""G28 G90 G94
G1 F1000 X{self.x_center:.0f} Y{self.y_center:.0f}  ; Move to center for testing

; Test 1: Low jerk response
M205 X50 Z25        ; Low jerk settings
M207                ; Report settings
G1 F2000 X{x_end:.0f} Y{self.y_center:.0f}   ; {move_distance:.0f}mm move
G4 P2
G1 X{x_start:.0f}             ; Return move

; Test 2: Medium jerk response  
M205 X200 Z100      ; Medium jerk settings
M207                ; Report settings
G1 Y{y_end:.0f}             ; {move_distance:.0f}mm move  
G4 P2
G1 Y{y_start:.0f}             ; Return move

; Test 3: High jerk response
M205 X500 Z250      ; High jerk settings
M207                ; Report settings
G1 X{x_end:.0f}             ; {move_distance:.0f}mm move
G4 P2
G1 X{x_start:.0f}             ; Return move

; Test 4: Multiplier effect
M205 X300 Z150      ; Reset to medium
M206 P0.5           ; Low multiplier
G1 Y{y_end:.0f}
G4 P1
G1 Y{y_start:.0f}
M206 P2.0           ; High multiplier  
G1 X{x_end:.0f}
G4 P1
G1 X{x_start:.0f}

; Reset and return home
M208
G28

; Expected Results:
; - Clearly distinguishable motion characteristics between settings
; - Low jerk: very smooth, potentially slower transitions
; - High jerk: more responsive, potentially some vibration
; - Multiplier effect: noticeable scaling of jerk behavior
; - All moves complete accurately without position loss
"""
        return content

    def generate_test_2a_jerk_optimization(self):
        """Jerk value optimization test using complex geometry"""
        # Use a complex pattern that stresses jerk limits
        x_start = self.x_center - self.medium_range/2
        x_end = self.x_center + self.medium_range/2
        y_start = self.y_center - self.medium_range/2
        y_end = self.y_center + self.medium_range/2
        x_mid = self.x_center
        y_mid = self.y_center
        
        content = self.header("Test 2A: Systematic Jerk Value Testing", 
                            "Tests jerk values from conservative to aggressive")
        
        content += f"""G28 G90 G94
M208                ; Reset to defaults

; Define test pattern (complex geometry to stress jerk limits)
; Move to start position
G1 F1000 X{x_start:.0f} Y{y_start:.0f}

; Test 1: Very Conservative Jerk (NEMA17 equivalent)
M205 X150 Z80       ; Conservative baseline
M207                ; Report settings
; Complex test pattern using {self.medium_range:.0f}mm range
G1 F3000 X{x_end:.0f} Y{y_start:.0f}    ; Fast linear move ({self.medium_range:.0f}mm)
G1 X{x_end:.0f} Y{y_end:.0f}            ; 90-degree corner ({self.medium_range:.0f}mm)
G1 X{x_start:.0f} Y{y_end:.0f}          ; Another corner ({self.medium_range:.0f}mm)
G1 X{x_start:.0f} Y{y_mid:.0f}          ; Shorter move ({self.medium_range/2:.0f}mm)
G1 X{x_mid:.0f} Y{y_mid:.0f}            ; Short move with direction change ({self.medium_range/2:.0f}mm)
G1 X{x_mid:.0f} Y{y_start:.0f}          ; Return to start ({self.medium_range/2:.0f}mm)
G4 P3               ; Pause for evaluation

; Test 2: NEMA23 Conservative
M205 X250 Z125      ; Higher torque motor capability
M207                ; Report settings
; Same pattern
G1 F3000 X{x_end:.0f} Y{y_start:.0f}
G1 X{x_end:.0f} Y{y_end:.0f}
G1 X{x_start:.0f} Y{y_end:.0f}
G1 X{x_start:.0f} Y{y_mid:.0f}
G1 X{x_mid:.0f} Y{y_mid:.0f}
G1 X{x_mid:.0f} Y{y_start:.0f}
G4 P3

; Test 3: NEMA23 Moderate
M205 X350 Z175      ; Moderate settings
M207
G1 F3000 X{x_end:.0f} Y{y_start:.0f}
G1 X{x_end:.0f} Y{y_end:.0f}
G1 X{x_start:.0f} Y{y_end:.0f}
G1 X{x_start:.0f} Y{y_mid:.0f}
G1 X{x_mid:.0f} Y{y_mid:.0f}
G1 X{x_mid:.0f} Y{y_start:.0f}
G4 P3

; Test 4: NEMA23/24 Aggressive
M205 X500 Z250      ; Aggressive for high-torque steppers
M207
G1 F3000 X{x_end:.0f} Y{y_start:.0f}
G1 X{x_end:.0f} Y{y_end:.0f}
G1 X{x_start:.0f} Y{y_end:.0f}
G1 X{x_start:.0f} Y{y_mid:.0f}
G1 X{x_mid:.0f} Y{y_mid:.0f}
G1 X{x_mid:.0f} Y{y_start:.0f}
G4 P3

; Test 5: Servo System Aggressive
M205 X700 Z350      ; Servo motor capabilities
M207
G1 F3000 X{x_end:.0f} Y{y_start:.0f}
G1 X{x_end:.0f} Y{y_end:.0f}
G1 X{x_start:.0f} Y{y_end:.0f}
G1 X{x_start:.0f} Y{y_mid:.0f}
G1 X{x_mid:.0f} Y{y_mid:.0f}
G1 X{x_mid:.0f} Y{y_start:.0f}
G4 P3

G28

; Evaluation Criteria:
; - Record vibration level (1-10 scale) for each test
; - Note any missed steps or position errors
; - Measure cycle time for each test sequence
; - Assess motion smoothness subjectively
; - Find highest jerk values before problems appear
"""
        return content

    def generate_test_stress_long_moves(self):
        """Stress test with long high-speed moves"""
        content = self.header("Test 3C: High-Speed Long Move Stress Test", 
                            "Tests S-curve performance on long moves using full travel envelope")
        
        # Use nearly full travel range for stress testing
        x_start = self.x_min + 10
        x_end = self.x_usable - 10
        y_start = self.y_min + 10
        y_end = self.y_usable - 10
        
        move_length = x_end - x_start
        
        content += f"""G28 G90 G94
M208                ; Reset to defaults

; Long-distance high-speed testing using {move_length:.0f}mm moves
; Tests S-curve performance at machine limits

; Test 1: Conservative settings for long moves
M205 X350 Z175      ; Moderate jerk
M206 P1.0 Q0.7 S1   ; Conservative S-curve
M210 F1.1 J1.0 A120 ; Gentle junction handling
M207

; Move to start position
G1 F2000 X{x_start:.0f} Y{y_start:.0f}

; Execute long-distance moves
G1 F4000 X{x_end:.0f} Y{y_start:.0f}     ; Full X travel ({move_length:.0f}mm)
G4 P1
G1 X{x_end:.0f} Y{y_end:.0f}             ; Full Y travel ({y_end - y_start:.0f}mm)
G4 P1
G1 X{x_start:.0f} Y{y_end:.0f}           ; Return X ({move_length:.0f}mm)
G4 P1
G1 X{x_start:.0f} Y{y_start:.0f}         ; Return Y ({y_end - y_start:.0f}mm)
G4 P2

; Test 2: Aggressive settings for maximum speed
M205 X600 Z300      ; High jerk for speed
M206 P1.4 Q0.8 S1   ; Aggressive S-curve
M210 F1.4 J1.2 A90  ; Fast cornering
M207

; Same long moves with aggressive settings
G1 F5000 X{x_end:.0f} Y{y_start:.0f}     ; High speed full travel
G4 P1
G1 X{x_end:.0f} Y{y_end:.0f}
G4 P1
G1 X{x_start:.0f} Y{y_end:.0f}
G4 P1
G1 X{x_start:.0f} Y{y_start:.0f}
G4 P2

; Test 3: Diagonal stress test
; Long diagonal moves stress both axes simultaneously
G1 F4000 X{x_end:.0f} Y{y_end:.0f}       ; Full diagonal ({math.sqrt(move_length**2 + (y_end-y_start)**2):.0f}mm)
G4 P1
G1 X{x_start:.0f} Y{y_start:.0f}         ; Return diagonal
G4 P1
G1 X{x_end:.0f} Y{y_start:.0f}           ; Opposite diagonal
G4 P1
G1 X{x_start:.0f} Y{y_end:.0f}           ; Complete pattern
G4 P2

G28

; Evaluation Points:
; - Monitor for missed steps on long moves
; - Check positioning accuracy after high-speed moves
; - Note any vibration or mechanical stress
; - Measure actual vs programmed move times
; - Verify S-curve smoothness maintained at high speeds
"""
        return content

    def generate_cutting_test_aluminum(self):
        """Aluminum cutting test with appropriate workpiece size"""
        # Use workpiece-appropriate coordinates
        workpiece_x = min(100, self.medium_range * 0.4)  # 40% of medium range or 100mm max
        workpiece_y = min(100, self.medium_range * 0.4)
        
        x_start = self.x_center - workpiece_x/2
        x_end = self.x_center + workpiece_x/2
        y_start = self.y_center - workpiece_y/2
        y_end = self.y_center + workpiece_y/2
        
        content = self.header("Test 5A: Aluminum Machining Optimization", 
                            f"Optimizes S-curve for aluminum cutting on {workpiece_x:.0f}x{workpiece_y:.0f}mm workpiece")
        
        content += f"""; SETUP REQUIRED: 6061 aluminum, appropriate end mill, speeds/feeds
; Workpiece positioned at machine center: X{self.x_center:.0f} Y{self.y_center:.0f}
; Test area: {workpiece_x:.0f}mm x {workpiece_y:.0f}mm

G28 G54 G90 G94
; M3 S18000         ; High speed for aluminum (ADJUST)

; Aluminum allows aggressive settings due to:
; - Easy machining characteristics
; - Good heat dissipation  
; - Minimal tool chatter concerns

; Test 1: Aggressive aluminum settings
M205 X500 Z250      ; High jerk for aluminum
M206 P1.4 Q0.8 S1   ; Aggressive motion
M210 F1.5 J1.3 A90  ; Fast cornering
M211 S1 P0.02 R3.0 V75 F0.8 L10  ; Optimized blending
M212 V60.0 Q3.0 D2.0 ; Quick positioning

G0 X{x_start:.0f} Y{y_start:.0f}
G0 Z2
G1 Z-2.0 F400       ; Deeper cut possible in aluminum

; High-speed aluminum roughing simulation
G1 X{x_end:.0f} Y{y_start:.0f} F1200     ; High feed rate ({workpiece_x:.0f}mm)
G1 Y{y_start + workpiece_y*0.2:.0f} F300 ; Step over
G1 X{x_start:.0f} F1200                  ; Rapid traverse
G1 Y{y_start + workpiece_y*0.4:.0f} F300
G1 X{x_end:.0f} F1200
G1 Y{y_start + workpiece_y*0.6:.0f} F300
G1 X{x_start:.0f} F1200
G1 Y{y_start + workpiece_y*0.8:.0f} F300
G1 X{x_end:.0f} F1200

; Aluminum finishing pass
G1 X{x_end:.0f} Y{y_end:.0f} F800        ; Finishing feed
G1 Y{y_start:.0f}
G1 X{x_start:.0f}
G1 Y{y_end:.0f}

G1 Z5
M5
G28

; Aluminum-Specific Metrics:
; - Surface finish quality at high feed rates
; - Corner sharpness with aggressive settings
; - Cycle time improvements vs quality
; - Tool life with optimized motion
; - Chip evacuation effectiveness
"""
        return content

    def generate_all_tests(self):
        """Generate all test files"""
        tests = {
            'test_1a_scurve_verification.gcode': self.generate_test_1a_comparison(),
            'test_1b_parameter_response.gcode': self.generate_test_1b_parameters(),
            'test_2a_jerk_optimization.gcode': self.generate_test_2a_jerk_optimization(),
            'test_3c_stress_long_moves.gcode': self.generate_test_stress_long_moves(),
            'test_5a_aluminum_cutting.gcode': self.generate_cutting_test_aluminum(),
        }
        return tests

def main():
    # Parse command line arguments
    if len(sys.argv) >= 4:
        try:
            x_max = float(sys.argv[1])
            y_max = float(sys.argv[2])
            z_max = float(sys.argv[3])
        except ValueError:
            print("Error: Travel limits must be numeric values")
            sys.exit(1)
    else:
        x_max, y_max, z_max = 500, 500, 200  # Default values
    
    print(f"Generating S-curve test G-code for {x_max}x{y_max}x{z_max}mm travel envelope")
    
    # Create generator
    generator = GCodeGenerator(x_max, y_max, z_max)
    
    # Create output directory
    output_dir = "test_gcode_generated"
    os.makedirs(output_dir, exist_ok=True)
    
    # Generate all test files
    tests = generator.generate_all_tests()
    
    for filename, content in tests.items():
        filepath = os.path.join(output_dir, filename)
        with open(filepath, 'w') as f:
            f.write(content)
        print(f"Generated: {filepath}")
    
    # Generate configuration summary
    config_file = os.path.join(output_dir, "configuration.txt")
    with open(config_file, 'w') as f:
        f.write(f"""S-Curve Test G-Code Configuration
=====================================

Machine Travel Limits:
- X-axis: 0 to {x_max}mm
- Y-axis: 0 to {y_max}mm  
- Z-axis: 0 to {z_max}mm

Safety Margins: {generator.safety_margin}mm
Usable Ranges:
- X: {generator.x_min}mm to {generator.x_usable}mm
- Y: {generator.y_min}mm to {generator.y_usable}mm
- Z: {generator.z_min}mm to {generator.z_usable}mm

Test Ranges:
- Small: {generator.small_range:.0f}mm
- Medium: {generator.medium_range:.0f}mm
- Large: {generator.large_range:.0f}mm

Center Point: X{generator.x_center:.0f} Y{generator.y_center:.0f}

Generated Files:
{chr(10).join(['- ' + f for f in tests.keys()])}

Usage Instructions:
1. Ensure your machine has the specified travel limits
2. Set work coordinate system G54 to use the full travel envelope
3. Verify safety margins are appropriate for your setup
4. Run tests in order: 1a, 1b, 2a, 3c, 5a
5. Document results in the testing guide
""")
    
    print(f"Configuration saved: {config_file}")
    print(f"\nGenerated {len(tests)} test files for {x_max}x{y_max}x{z_max}mm envelope")
    print("Files are ready for CNC testing with proper S-curve optimization.")

if __name__ == "__main__":
    main()