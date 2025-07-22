; Test 1A: S-curve vs Linear Comparison
; This test compares motion with and without S-curve

M208                ; Reset to defaults
G28                 ; Home
G90 G94             ; Absolute, feed rate mode

; Test without S-curve (disable if possible, or use minimal jerk)
M205 X5 Z2          ; Very low jerk approximates linear
G1 F1000 X50 Y50    ; Move to start position
G4 P1               ; Pause 1 second

; Execute test pattern
G1 F3000 X100 Y0    ; Fast move in X
G1 Y100             ; Fast move in Y  
G1 X0               ; Fast move back
G1 Y0               ; Return to start

; Now test with proper S-curve
M205 X150 Z80       ; Standard S-curve settings
G4 P2               ; Pause
G1 F3000 X100 Y0    ; Same pattern with S-curve
G1 Y100
G1 X0
G1 Y0

; Return home
G28