; Test 5B: CNC Machining Test Pattern
; Simulates typical machining operations

M208 G28 G90 G94
; CNC settings
M204 P4000 T2000
M205 X25 Z12
M206 M1.2 C0.8 A1

; Roughing pass simulation
G1 F1500 Z-1        ; Plunge
G1 F2400 X10 Y10    ; Roughing moves
G1 X90
G1 Y15
G1 X10
G1 Y20
G1 X90
G1 Y25
G1 X10

; Finishing pass
G1 F1800 X10 Y10    ; Slower, more precise
G1 X90 Y10
G1 Y30
G1 X10
G1 Y10

; Profile cutting
G1 X50 Y15
G1 X85 Y25          ; Angled cut
G1 Y35
G1 X15
G1 Y25
G1 X50 Y15

; Pocket clearing
G1 X30 Y40
G1 X70 Y40
G1 Y70
G1 X30
G1 Y40

G1 Z1               ; Retract
G28