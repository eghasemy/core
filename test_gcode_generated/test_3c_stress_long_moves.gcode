; Test 3C: High-Speed Long Move Stress Test
; Tests S-curve performance on long moves using full travel envelope
; Generated for travel envelope: 300.0x300.0x150.0mm
; Usable range: X25-275.0, Y25-275.0, Z5-100

G28 G90 G94
M208                ; Reset to defaults

; Long-distance high-speed testing using 230mm moves
; Tests S-curve performance at machine limits

; Test 1: Conservative settings for long moves
M205 X350 Z175      ; Moderate jerk
M206 P1.0 Q0.7 S1   ; Conservative S-curve
M210 F1.1 J1.0 A120 ; Gentle junction handling
M207

; Move to start position
G1 F2000 X35 Y35

; Execute long-distance moves
G1 F4000 X265 Y35     ; Full X travel (230mm)
G4 P1
G1 X265 Y265             ; Full Y travel (230mm)
G4 P1
G1 X35 Y265           ; Return X (230mm)
G4 P1
G1 X35 Y35         ; Return Y (230mm)
G4 P2

; Test 2: Aggressive settings for maximum speed
M205 X600 Z300      ; High jerk for speed
M206 P1.4 Q0.8 S1   ; Aggressive S-curve
M210 F1.4 J1.2 A90  ; Fast cornering
M207

; Same long moves with aggressive settings
G1 F5000 X265 Y35     ; High speed full travel
G4 P1
G1 X265 Y265
G4 P1
G1 X35 Y265
G4 P1
G1 X35 Y35
G4 P2

; Test 3: Diagonal stress test
; Long diagonal moves stress both axes simultaneously
G1 F4000 X265 Y265       ; Full diagonal (325mm)
G4 P1
G1 X35 Y35         ; Return diagonal
G4 P1
G1 X265 Y35           ; Opposite diagonal
G4 P1
G1 X35 Y265           ; Complete pattern
G4 P2

G28

; Evaluation Points:
; - Monitor for missed steps on long moves
; - Check positioning accuracy after high-speed moves
; - Note any vibration or mechanical stress
; - Measure actual vs programmed move times
; - Verify S-curve smoothness maintained at high speeds
