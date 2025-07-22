; Test 1A: S-curve vs Linear Motion Comparison for CNC Machines
; Verifies S-curve system is functioning correctly on higher-end CNC machines
; For NEMA23/24 steppers or servo systems

M208                ; Reset to defaults
G28                 ; Home all axes
G90 G94             ; Absolute, feed rate mode

; Test 1: Minimal S-curve (approximates linear acceleration)
M205 X50 Z25        ; Very low jerk settings
M206 M0.8 C1.0      ; Minimal S-curve effect
G1 F1000 X50 Y50    ; Move to start position
G4 P2               ; Pause for observation

; Execute test pattern with minimal S-curve
G1 F3000 X150 Y50   ; Fast move in X (100mm travel)
G4 P1
G1 Y150             ; Fast move in Y (100mm travel)  
G4 P1
G1 X50              ; Return move (100mm travel)
G4 P1
G1 Y50              ; Complete square (100mm travel)
G4 P2

; Test 2: Proper S-curve settings for CNC
M205 X400 Z200      ; NEMA23/24 capable jerk values
M206 M1.2 C0.7      ; Full S-curve features enabled
G4 P2               ; Pause for comparison

; Execute same pattern with full S-curve
G1 F3000 X150 Y50   ; Same moves with S-curve enabled
G4 P1
G1 Y150
G4 P1  
G1 X50
G4 P1
G1 Y50
G4 P2

; Test 3: Aggressive servo-capable settings (if applicable)
M205 X600 Z300      ; Servo motor capable values
M206 M1.4 C0.8      ; Aggressive S-curve optimization
G4 P2

; Execute pattern with servo-optimized S-curve
G1 F4000 X150 Y50   ; Higher speed possible with servos
G4 P1
G1 Y150
G4 P1  
G1 X50
G4 P1
G1 Y50
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