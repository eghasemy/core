; Path Blending Test
; Tests S-curve path blending with various tolerances and geometries
;
; Usage: Test different blending settings to observe smoothing effects
; Start with conservative settings:
; M211 S1 P0.01 R1.0 V5 F0.8 L6

G21 G90 G94  ; mm, absolute, feed rate mode
G28          ; Home all axes

; Test 1: Baseline - No blending
M211 S0      ; Disable path blending
F1000

G0 X0 Y0
G1 X20 Y0
G1 X20 Y10
G1 X10 Y10
G1 X10 Y20
G1 X0 Y20
G1 X0 Y0

G4 P3        ; Pause to observe sharp corners

; Test 2: Conservative blending
M211 S1 P0.01 R0.5 V5 F0.8 L6

G0 X30 Y0
G1 X50 Y0
G1 X50 Y10
G1 X40 Y10
G1 X40 Y20
G1 X30 Y20
G1 X30 Y0

G4 P3        ; Pause to observe blended motion

; Test 3: Moderate blending
M211 S1 P0.02 R1.0 V10 F0.6 L8

G0 X60 Y0
G1 X80 Y0
G1 X80 Y10
G1 X70 Y10
G1 X70 Y20
G1 X60 Y20
G1 X60 Y0

G4 P3        ; Pause to observe smoother blending

; Test 4: Aggressive blending
M211 S1 P0.05 R2.0 V15 F0.5 L10

G0 X90 Y0
G1 X110 Y0
G1 X110 Y10
G1 X100 Y10
G1 X100 Y20
G1 X90 Y20
G1 X90 Y0

G4 P3        ; Pause to observe very smooth blending

; Test 5: Circular-like path (blend radius test)
M211 S1 P0.02 R1.5 V10 F0.6 L8

G0 X0 Y30
F800
G1 X5 Y30    ; Start of pseudo-circle
G1 X10 Y32
G1 X14 Y35
G1 X17 Y39
G1 X18 Y44   ; Top
G1 X17 Y49
G1 X14 Y53
G1 X10 Y56
G1 X5 Y58
G1 X0 Y58    ; Left side
G1 X-5 Y56
G1 X-9 Y53
G1 X-12 Y49
G1 X-13 Y44  ; Bottom
G1 X-12 Y39
G1 X-9 Y35
G1 X-5 Y32
G1 X0 Y30    ; Back to start

G4 P3

; Test 6: High-frequency direction changes (blending stress test)
M211 S1 P0.01 R0.8 V8 F0.7 L6

G0 X30 Y30
F600
G1 X35 Y30
G1 X35 Y32   ; Start of zigzag
G1 X37 Y32
G1 X37 Y34
G1 X39 Y34
G1 X39 Y36
G1 X41 Y36
G1 X41 Y38
G1 X43 Y38
G1 X43 Y40
G1 X45 Y40
G1 X45 Y42
G1 X47 Y42
G1 X47 Y44
G1 X49 Y44
G1 X49 Y46
G1 X51 Y46

G4 P3

; Test 7: Speed variation with blending
M211 S1 P0.02 R1.2 V5 F0.6 L8

G0 X60 Y30
F2000        ; High speed entry
G1 X70 Y30
F300         ; Slow through complex section
G1 X70 Y35
G1 X75 Y35
G1 X75 Y40
G1 X80 Y40
G1 X80 Y45
F2000        ; High speed exit
G1 X90 Y45

G4 P3

; Test 8: Tolerance test - very tight blending
M211 S1 P0.005 R0.3 V3 F0.9 L4

G0 X0 Y60
F400
G1 X8 Y60    ; Very precise blending test
G1 X8 Y65
G1 X12 Y65
G1 X12 Y70
G1 X16 Y70
G1 X16 Y75
G1 X20 Y75
G1 X20 Y80
G1 X12 Y80
G1 X12 Y75
G1 X8 Y75
G1 X8 Y70
G1 X4 Y70
G1 X4 Y65
G1 X0 Y65
G1 X0 Y60

G4 P3

; Test 9: Lookahead optimization test
M211 S1 P0.02 R1.5 V12 F0.5 L12  ; Maximum lookahead

G0 X30 Y60
F1200
; Long sequence for lookahead optimization
G1 X32 Y60
G1 X34 Y62
G1 X36 Y62
G1 X38 Y64
G1 X40 Y64
G1 X42 Y66
G1 X44 Y66
G1 X46 Y68
G1 X48 Y68
G1 X50 Y70
G1 X52 Y70
G1 X54 Y72
G1 X56 Y72
G1 X58 Y74
G1 X60 Y74
G1 X62 Y76

G4 P3

; Test 10: Blend factor variation
G0 X70 Y60

; High jerk factor (less smooth)
M211 S1 P0.02 R1.0 V10 F0.9 L8
F800
G1 X75 Y60
G1 X75 Y65
G1 X80 Y65
G1 X80 Y70

G4 P1

; Low jerk factor (very smooth)
M211 S1 P0.02 R1.0 V10 F0.3 L8
G1 X85 Y70
G1 X85 Y75
G1 X90 Y75
G1 X90 Y80

; Reset to defaults
M211 S1 P0.02 R2.0 V10 F0.6 L8
M210 F1.2 J0.8 A150

; Return to origin
G0 X0 Y0

; Test complete
M2