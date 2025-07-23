; Test 1B: Parameter Response Verification
; Tests immediate response to parameter modifications
; Generated for travel envelope: 300.0x300.0x150.0mm
; Usable range: X25-275.0, Y25-275.0, Z5-100

G28 G90 G94
G1 F1000 X150 Y150  ; Move to center for testing

; Test 1: Low jerk response
M205 X50 Z25        ; Low jerk settings
M207                ; Report settings
G1 F2000 X188 Y150   ; 75mm move
G4 P2
G1 X112             ; Return move

; Test 2: Medium jerk response  
M205 X200 Z100      ; Medium jerk settings
M207                ; Report settings
G1 Y188             ; 75mm move  
G4 P2
G1 Y112             ; Return move

; Test 3: High jerk response
M205 X500 Z250      ; High jerk settings
M207                ; Report settings
G1 X188             ; 75mm move
G4 P2
G1 X112             ; Return move

; Test 4: Multiplier effect
M205 X300 Z150      ; Reset to medium
M206 P0.5           ; Low multiplier
G1 Y188
G4 P1
G1 Y112
M206 P2.0           ; High multiplier  
G1 X188
G4 P1
G1 X112

; Reset and return home
M208
G28

; Expected Results:
; - Clearly distinguishable motion characteristics between settings
; - Low jerk: very smooth, potentially slower transitions
; - High jerk: more responsive, potentially some vibration
; - Multiplier effect: noticeable scaling of jerk behavior
; - All moves complete accurately without position loss
