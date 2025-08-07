; Test 1A: S-curve vs Linear Motion Comparison
; Verifies S-curve system is functioning correctly
; Generated for travel envelope: 300.0x300.0x150.0mm
; Usable range: X25-275.0, Y25-275.0, Z5-100

M208                ; Reset to defaults
G28                 ; Home all axes
G90 G94             ; Absolute, feed rate mode

; Test 1: Very low jerk (approximates linear)
M205 X25 Z15        ; Minimal jerk settings
G1 F1000 X75 Y75  ; Move to start position
G4 P2               ; Pause for observation

; Execute test pattern with minimal S-curve - 150mm square
G1 F2500 X225 Y75   ; Fast move in X (150mm travel)
G4 P1
G1 Y225             ; Fast move in Y (150mm travel)  
G4 P1
G1 X75             ; Return move (150mm travel)
G4 P1
G1 Y75             ; Complete square (150mm travel)
G4 P2

; Test 2: Proper S-curve settings
M205 X300 Z150      ; Proper S-curve jerk values
M206 P1.2 Q0.7      ; Enable full S-curve features
G4 P2               ; Pause for comparison

; Execute same pattern with full S-curve
G1 F2500 X225 Y75   ; Same moves with S-curve enabled
G4 P1
G1 Y225
G4 P1  
G1 X75
G4 P1
G1 Y75
G4 P2

; Return to home
G28
M207                ; Report final settings

; Expected observations:
; - Test 1: More abrupt motion, potential vibration
; - Test 2: Smoother acceleration/deceleration, less vibration  
; - No missed steps or position errors throughout
; - Clear audible differences in motor sound characteristics
