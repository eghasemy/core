; Test 3C: High-Speed Stress Test
; Pushes S-curve to limits

M208 G28 G90 G94

; Progressive speed increase
G1 F1000 X10 Y10

; Speed test 1: 3000 mm/min
G1 F3000
G1 X90 Y10
G1 X90 Y90
G1 X10 Y90
G1 X10 Y10

; Speed test 2: 6000 mm/min
G1 F6000
G1 X90 Y10
G1 X90 Y90
G1 X10 Y90
G1 X10 Y10

; Speed test 3: 9000 mm/min (if machine capable)
G1 F9000
G1 X90 Y10
G1 X90 Y90
G1 X10 Y90
G1 X10 Y10

; Complex pattern at high speed
G1 F6000
G1 X50 Y10
G1 X90 Y30
G1 X70 Y50
G1 X90 Y70
G1 X50 Y90
G1 X10 Y70
G1 X30 Y50
G1 X10 Y30
G1 X50 Y10

G28