; Test 1A: S-curve vs Linear Motion Comparison for CNC Machines
; Verifies S-curve system is functioning correctly on higher-end CNC machines
; For NEMA23/24 steppers or servo systems
; Designed for 500x500x200mm travel envelope

M208                ; Reset to defaults
G28                 ; Home all axes
G90 G94             ; Absolute, feed rate mode

; Test 1: Minimal S-curve (approximates linear acceleration)
M205 X50 Z25        ; Very low jerk settings
M206 P0.8 Q1.0      ; Minimal S-curve effect
G1 F1000 X150 Y150  ; Move to start position (center area)
G4 P2               ; Pause for observation

; Execute test pattern with minimal S-curve - 300mm square
G1 F3000 X450 Y150  ; Fast move in X (300mm travel)
G4 P1
G1 Y450             ; Fast move in Y (300mm travel)  
G4 P1
G1 X150             ; Return move (300mm travel)
G4 P1
G1 Y150             ; Complete square (300mm travel)
G4 P2

; Test 2: Proper S-curve settings for CNC
M205 X400 Z200      ; NEMA23/24 capable jerk values
M206 P1.2 Q0.7      ; Full S-curve features enabled
G4 P2               ; Pause for comparison

; Execute same pattern with full S-curve
G1 F3000 X450 Y150  ; Same moves with S-curve enabled
G4 P1
G1 Y450
G4 P1  
G1 X150
G4 P1
G1 Y150
G4 P2

; Test 3: Aggressive servo-capable settings (if applicable)
M205 X600 Z300      ; Servo motor capable values
M206 P1.4 Q0.8      ; Aggressive S-curve optimization
G4 P2

; Execute pattern with servo-optimized S-curve
G1 F4000 X450 Y150  ; Higher speed possible with servos
G4 P1
G1 Y450
G4 P1  
G1 X150
G4 P1
G1 Y150
G4 P2

; Return to home and report
G28
M207                ; Report final settings

; Expected observations:
; - Test 1: More abrupt motion, potential vibration
; - Test 2: Smoother acceleration/deceleration, less vibration  
; - Test 3: Very smooth high-speed motion (servo systems only)
; - No missed steps or position errors throughout
; - Clear audible differences in motor sound characteristics