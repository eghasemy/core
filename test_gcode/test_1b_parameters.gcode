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