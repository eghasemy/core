; Test 4A: Adaptive Jerk Behavior
; Tests adaptive jerk response to different move types

M208 G28 G90 G94
M205 X150 Z80
M206 A1             ; Enable adaptive jerk

G1 F2000 X10 Y10

; Long moves (should use higher jerk)
G1 X90 Y10          ; 80mm move
G4 P0.5
G1 Y90              ; 80mm move
G4 P0.5

; Short moves (should use lower jerk)
G1 X85 Y85          ; 7mm move
G1 X80 Y90          ; 7mm move
G1 X85 Y85          ; 7mm move

; Mixed pattern
G1 X10 Y50          ; Long move
G1 X15 Y55          ; Short move
G1 X90 Y55          ; Long move
G1 X85 Y50          ; Short move

; Sharp corners (should reduce jerk)
G1 X90 Y10
G1 X10 Y10          ; 180-degree reversal
G1 X10 Y90
G1 X90 Y90          ; 90-degree corner

; Test without adaptive
M206 A0
G1 X50 Y10
G1 X10 Y10          ; Same moves without adaptive
G1 X10 Y50
G1 X50 Y50

G28