; Junction Velocity Optimization Test
; Tests S-curve junction velocity optimization with various corner angles
;
; Usage: Run with path blending enabled to see smoothing effects
; M211 S1 P0.02 R2.0 V10 F0.6 L8  ; Enable path blending
; M210 F1.2 J0.8 A150             ; Set junction optimization

G21 G90 G94  ; mm, absolute, feed rate mode
G28          ; Home all axes
F1000        ; Set feed rate

; Test 1: Sharp 90-degree corners (high jerk stress)
G0 X0 Y0
G1 X20 Y0
G1 X20 Y20   ; 90-degree corner
G1 X0 Y20
G1 X0 Y0

; Pause to observe motion
G4 P2

; Test 2: 45-degree corners (moderate jerk)
G0 X30 Y0
G1 X50 Y0
G1 X60 Y10   ; 45-degree corner
G1 X70 Y20
G1 X50 Y20
G1 X30 Y0

; Pause to observe motion
G4 P2

; Test 3: Shallow angle corners (minimal jerk)
G0 X80 Y0
G1 X100 Y0
G1 X105 Y2   ; ~11-degree corner
G1 X110 Y4
G1 X115 Y6
G1 X120 Y8
G1 X100 Y8
G1 X80 Y0

; Pause to observe motion
G4 P2

; Test 4: Rapid direction changes (junction stress test)
G0 X0 Y30
G1 X10 Y30
G1 X10 Y40   ; 90-degree corner
G1 X20 Y40
G1 X20 Y30   ; 90-degree corner
G1 X30 Y30
G1 X30 Y40   ; 90-degree corner
G1 X40 Y40
G1 X40 Y30   ; 90-degree corner
G1 X50 Y30

; Pause to observe motion
G4 P2

; Test 5: Micro-moves with junctions (precision test)
G0 X60 Y30
G91          ; Incremental mode
G1 X1 Y0
G1 X1 Y1     ; Many small junction moves
G1 X1 Y0
G1 X1 Y1
G1 X1 Y0
G1 X1 Y1
G1 X1 Y0
G1 X1 Y1
G1 X1 Y0
G1 X1 Y1
G90          ; Back to absolute

; Test 6: Speed variation through junctions
G0 X0 Y50
F2000        ; High speed
G1 X20 Y50
F500         ; Slow through corner
G1 X20 Y70
F2000        ; High speed again
G1 X0 Y70
F500         ; Slow through corner
G1 X0 Y50

; Test 7: Complex path with varying junction angles
G0 X30 Y50
F1000
G1 X35 Y50   ; Start of complex path
G1 X40 Y55   ; 45-degree junction
G1 X45 Y55   ; Straight
G1 X50 Y60   ; 45-degree junction
G1 X55 Y58   ; Shallow angle
G1 X60 Y62   ; Moderate angle
G1 X65 Y60   ; Reverse direction change
G1 X70 Y65   ; Final segment

; Return to origin
G0 X0 Y0

; Test complete
M2