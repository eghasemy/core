; Lookahead Optimization Test
; Tests multi-block lookahead optimization for S-curve motion
;
; Usage: Test with different lookahead block counts
; M211 L4   ; Small lookahead
; M211 L8   ; Medium lookahead  
; M211 L12  ; Large lookahead

G21 G90 G94  ; mm, absolute, feed rate mode
G28          ; Home all axes

; Test 1: Short lookahead (4 blocks)
M211 S1 P0.02 R1.5 V10 F0.6 L4
M210 F1.2 J0.8 A150

G0 X0 Y0
F1000

; Simple sequence - should not benefit much from lookahead
G1 X10 Y0
G1 X10 Y10
G1 X0 Y10
G1 X0 Y0

G4 P2

; Test 2: Medium lookahead (8 blocks)
M211 L8

G0 X20 Y0
; More complex sequence - should benefit from medium lookahead
G1 X25 Y0
G1 X25 Y2
G1 X30 Y2
G1 X30 Y4
G1 X35 Y4
G1 X35 Y6
G1 X40 Y6
G1 X40 Y8
G1 X45 Y8
G1 X45 Y10
G1 X40 Y10
G1 X35 Y10
G1 X30 Y10
G1 X25 Y10
G1 X20 Y10
G1 X20 Y0

G4 P2

; Test 3: Large lookahead (12 blocks)
M211 L12

G0 X50 Y0
; Long complex sequence - should benefit most from large lookahead
G1 X52 Y0
G1 X54 Y1
G1 X56 Y1
G1 X58 Y2
G1 X60 Y2
G1 X62 Y3
G1 X64 Y3
G1 X66 Y4
G1 X68 Y4
G1 X70 Y5
G1 X72 Y5
G1 X74 Y6
G1 X76 Y6
G1 X78 Y7
G1 X80 Y7
G1 X82 Y8
G1 X84 Y8
G1 X86 Y9
G1 X88 Y9
G1 X90 Y10
; Return path
G1 X88 Y11
G1 X86 Y11
G1 X84 Y12
G1 X82 Y12
G1 X80 Y13
G1 X78 Y13
G1 X76 Y14
G1 X74 Y14
G1 X72 Y15
G1 X70 Y15
G1 X68 Y16
G1 X66 Y16
G1 X64 Y17
G1 X62 Y17
G1 X60 Y18
G1 X58 Y18
G1 X56 Y19
G1 X54 Y19
G1 X52 Y20
G1 X50 Y20
G1 X50 Y0

G4 P3

; Test 4: Comparison test - same path with different lookahead
; This demonstrates the impact of lookahead on motion optimization

; Small lookahead version
M211 L4
G0 X0 Y30
F1200

G1 X5 Y30
G1 X10 Y32
G1 X15 Y34
G1 X20 Y36
G1 X25 Y38
G1 X30 Y40
G1 X35 Y42
G1 X40 Y44
G1 X45 Y46
G1 X50 Y48
G1 X55 Y50

G4 P2

; Large lookahead version (same path)
M211 L12
G0 X0 Y35

G1 X5 Y35
G1 X10 Y37
G1 X15 Y39
G1 X20 Y41
G1 X25 Y43
G1 X30 Y45
G1 X35 Y47
G1 X40 Y49
G1 X45 Y51
G1 X50 Y53
G1 X55 Y55

G4 P3

; Test 5: Sharp corners with lookahead optimization
; Tests adaptive jerk based on lookahead analysis

M211 L10
G0 X70 Y30
F800

; Sequence with mix of sharp and gentle corners
G1 X75 Y30   ; Start
G1 X75 Y35   ; 90-degree corner (should reduce jerk)
G1 X80 Y35   ; Straight (should maintain jerk)
G1 X82 Y37   ; Gentle corner (should allow higher jerk)
G1 X84 Y39   ; Gentle corner
G1 X86 Y41   ; Gentle corner
G1 X88 Y43   ; Gentle corner
G1 X88 Y48   ; 90-degree corner (should reduce jerk)
G1 X83 Y48   ; Straight
G1 X83 Y53   ; 90-degree corner (should reduce jerk)
G1 X78 Y53   ; Straight
G1 X78 Y48   ; 90-degree corner
G1 X73 Y48   ; Straight
G1 X73 Y43   ; 90-degree corner
G1 X70 Y43   ; Straight
G1 X70 Y30   ; Return

G4 P3

; Test 6: Long straight segments (should enable higher jerk)
M211 L8
G0 X0 Y60
F1500

; Long straight moves - lookahead should detect and optimize
G1 X20 Y60   ; Long straight - should allow high jerk
G1 X20 Y80   ; Long straight - should allow high jerk
G1 X40 Y80   ; Long straight - should allow high jerk
G1 X40 Y60   ; Long straight - should allow high jerk
G1 X0 Y60    ; Return

G4 P2

; Test 7: Mixed short and long moves
M211 L10
G0 X50 Y60
F1000

; Mixed sequence - lookahead should optimize accordingly
G1 X52 Y60   ; Short move - should use conservative jerk
G1 X52 Y62   ; Short move - should use conservative jerk
G1 X72 Y62   ; Long move - should allow higher jerk
G1 X72 Y64   ; Short move - should reduce jerk again
G1 X74 Y64   ; Short move
G1 X74 Y84   ; Long move - should increase jerk
G1 X76 Y84   ; Short move - should reduce jerk
G1 X76 Y86   ; Short move
G1 X56 Y86   ; Long move - should increase jerk
G1 X56 Y88   ; Short move
G1 X54 Y88   ; Short move
G1 X54 Y68   ; Long move - should increase jerk
G1 X52 Y68   ; Short move
G1 X50 Y68   ; Short move
G1 X50 Y60   ; Return

G4 P3

; Test 8: Circular motion with lookahead
M211 L12
G0 X20 Y70
F600

; Circular-like motion - lookahead should optimize for smooth curves
G1 X25 Y70
G1 X29 Y72
G1 X32 Y75
G1 X34 Y79
G1 X35 Y84   ; Top of circle
G1 X34 Y89
G1 X32 Y93
G1 X29 Y96
G1 X25 Y98
G1 X20 Y99   ; Left side
G1 X15 Y98
G1 X11 Y96
G1 X8 Y93
G1 X6 Y89
G1 X5 Y84    ; Bottom
G1 X6 Y79
G1 X8 Y75
G1 X11 Y72
G1 X15 Y70
G1 X20 Y70   ; Complete circle

; Test 9: Stress test - rapid direction changes
M211 L6  ; Smaller lookahead for rapid changes
G0 X80 Y70
F400

; Rapid direction changes - lookahead should detect complexity
G1 X82 Y70
G1 X82 Y72   ; 90-degree
G1 X84 Y72
G1 X84 Y70   ; 90-degree  
G1 X86 Y70
G1 X86 Y72   ; 90-degree
G1 X88 Y72
G1 X88 Y74   ; 90-degree
G1 X86 Y74
G1 X86 Y76   ; 90-degree
G1 X84 Y76
G1 X84 Y74   ; 90-degree
G1 X82 Y74
G1 X82 Y76   ; 90-degree
G1 X80 Y76
G1 X80 Y70   ; Return

; Reset to optimal defaults
M211 S1 P0.02 R2.0 V10 F0.6 L8
M210 F1.2 J0.8 A150

; Return to origin
G0 X0 Y0

; Test complete  
M2