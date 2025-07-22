; Test 2B: Surface Quality Test
; Creates test surfaces for roughness measurement

M208 G28 G90 G94

; Test patch 1: Conservative settings
M205 X30 Z15
M206 M0.7 C0.5
G1 F1500 X10 Y10
; Create 20x20mm test patch
G1 X30 Y10
G1 X30 Y30
G1 X10 Y30
G1 X10 Y10

; Test patch 2: Standard settings  
M205 X150 Z80
M206 M1.0 C0.7
G1 X40 Y10
G1 X60 Y10
G1 X60 Y30
G1 X40 Y30
G1 X40 Y10

; Test patch 3: Aggressive settings
M205 X300 Z150
M206 M1.3 C0.9
G1 X70 Y10
G1 X90 Y10
G1 X90 Y30
G1 X70 Y30
G1 X70 Y10

G28