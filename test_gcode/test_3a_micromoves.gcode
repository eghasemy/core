; Test 3A: Micro-Moves Test
; Tests very small moves where S-curve might struggle

M208 G28 G90 G94
M205 X150 Z80       ; Standard settings

G1 F1000 X50 Y50    ; Move to center

; Micro-moves (0.1mm steps)
G1 X50.1
G1 Y50.1
G1 X50.0
G1 Y50.0

; Small moves (1mm steps)
G1 X51
G1 Y51
G1 X50
G1 Y50

; Very fast micro-moves
G1 F5000
G1 X50.1 Y50.1
G1 X50.0 Y50.0
G1 X50.1 Y50.1
G1 X50.0 Y50.0

G28