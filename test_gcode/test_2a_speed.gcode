; Test 2A: Speed Performance Test
; Measures time to complete complex path

M208 G28 G90 G94
M205 X100 Z50       ; Baseline settings

; Record start time manually or use timer
; Complex path test
G1 F4000 X10 Y10
G1 X90 Y10
G1 X90 Y90  
G1 X10 Y90
G1 X10 Y10

; Circle approximation (octagon)
G1 X50 Y10
G1 X78.54 Y21.46    ; 8-sided polygon
G1 X90 Y50
G1 X78.54 Y78.54
G1 X50 Y90
G1 X21.46 Y78.54
G1 X10 Y50
G1 X21.46 Y21.46
G1 X50 Y10

; Zig-zag pattern
G1 X10 Y20
G1 X90 Y30
G1 X10 Y40
G1 X90 Y50
G1 X10 Y60
G1 X90 Y70
G1 X10 Y80

G28
; Record end time