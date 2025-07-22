; Test 5A: 3D Printing Test Pattern
; Simulates typical 3D printing moves

M208 G28 G90 G94
; 3D printing settings
M204 P3000 R5000 T1500
M205 X20 Z8 E40
M206 M1.0 C0.7 A1

; Simulate print moves
G1 F1800 X10 Y10    ; Travel move
; Perimeter simulation
G1 F1200 X90 Y10    ; Outer perimeter
G1 Y90
G1 X10
G1 Y10

; Inner perimeter
G1 X20 Y20
G1 X80 Y20
G1 Y80
G1 X20
G1 Y20

; Infill simulation (diagonal lines)
G1 F2400 X25 Y25
G1 X75 Y75
G1 X25 Y75
G1 X75 Y25
G1 X25 Y50
G1 X75 Y50

; Small details
G1 F600 X40 Y40
G1 X45 Y40
G1 X45 Y45
G1 X40 Y45
G1 X40 Y40

G28