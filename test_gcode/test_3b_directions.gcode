; Test 3B: Sharp Direction Changes
; Tests rapid direction reversals

M208 G28 G90 G94
M205 X150 Z80

G1 F2000 X50 Y50

; Rapid reversals
G1 X60
G1 X40
G1 X60
G1 X40
G1 X50

; 90-degree corners
G1 Y60
G1 X60
G1 Y40
G1 X40
G1 Y50

; Acute angles
G1 X60 Y60
G1 X40 Y60
G1 X60 Y40
G1 X40 Y40
G1 X50 Y50

G28