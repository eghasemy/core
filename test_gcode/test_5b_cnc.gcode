; Test 5B: CNC Machining Performance Test
; Optimized test pattern for CNC machining operations
; Tests various machining scenarios with S-curve acceleration

M208 G28 G90 G94

; CNC-optimized S-curve settings for testing
M204 P8000 T4000    ; Aggressive acceleration for NEMA23/24
M205 X400 Z200      ; Higher jerk values for CNC machines  
M206 P1.2 Q0.7 S1   ; Balanced S-curve settings

; Test 1: Roughing operation simulation
; Rapid positioning with heavy cutting simulation
G1 F1000 X20 Y20    ; Position for roughing
G1 F2500 X80 Y20    ; Rapid traverse (roughing pass)
G1 Y25 F400         ; Plunge rate simulation
G1 X20 F2500        ; Rapid traverse return
G1 Y30 F400         ; Next plunge
G1 X80 F2500        ; Traverse
G1 Y35 F400
G1 X20 F2500
G1 Y40 F400
G1 X80 F2500

; Test 2: Finishing operation simulation  
; Precision moves with surface finish priority
G1 F1200 X20 Y40    ; Finishing pass start
G1 X80 Y40          ; Finishing traverse (slower for quality)
G1 Y20              ; Side finishing pass
G1 X20              ; Complete finish boundary
G1 Y40              ; Return to start

; Test 3: Profile cutting simulation
; Complex geometry with corner accuracy testing
G1 F1800 X30 Y25    ; Move to profile start
G1 X70 Y35          ; Angled cut (45 degrees)
G1 Y45              ; Vertical cut
G1 X60 Y55          ; Another angle
G1 X40 Y55          ; Horizontal cut
G1 X30 Y45          ; Return angle
G1 Y35              ; Vertical return
G1 X30 Y25          ; Close profile

; Test 4: Pocket clearing simulation
; Rapid direction changes with positioning accuracy
G1 F2000 X25 Y50    ; Position for pocket
G1 X75 Y50          ; Pocket boundary
G1 Y70              ; Pocket side
G1 X25              ; Return side
G1 Y50              ; Complete pocket outline

; Pocket clearing pattern (zigzag)
G1 X30 Y55 F1500    ; Enter pocket
G1 Y65              ; Clear pass
G1 X35              ; Step over
G1 Y55              ; Return pass
G1 X40              ; Step over
G1 Y65              ; Forward pass
G1 X45              ; Continue pattern
G1 Y55
G1 X50
G1 Y65
G1 X55
G1 Y55
G1 X60
G1 Y65
G1 X65
G1 Y55
G1 X70
G1 Y65

; Test 5: High-speed contouring
; Complex curves testing S-curve performance
G1 F3000 X50 Y30    ; Position for contour
; Simulate complex 3D profiling moves
G1 X55 Y35          ; Gradual curves
G1 X65 Y38
G1 X75 Y40
G1 X85 Y38
G1 X90 Y35
G1 X85 Y32
G1 X75 Y30
G1 X65 Y32
G1 X55 Y35
G1 X50 Y30

; Test 6: Drilling cycle simulation
; Z-axis intensive testing
G1 X60 Y60 F2000    ; Position for drilling
G1 Z-2 F300         ; Drill plunge
G1 Z5 F1000         ; Rapid retract
G1 X70 Y60          ; Next hole position
G1 Z-2 F300         ; Drill plunge
G1 Z5 F1000         ; Rapid retract
G1 X60 Y70          ; Next hole
G1 Z-2 F300
G1 Z5 F1000
G1 X70 Y70          ; Final hole
G1 Z-2 F300
G1 Z5 F1000

; Return to home
G28

; Performance evaluation points:
; - Roughing: Check rapid traverse smoothness and positioning accuracy
; - Finishing: Assess surface finish quality and corner precision
; - Profiling: Validate complex geometry handling and corner accuracy
; - Pocketing: Test rapid direction changes and consistent motion
; - Contouring: Evaluate high-speed curve following performance
; - Drilling: Check Z-axis motion optimization and repeatability