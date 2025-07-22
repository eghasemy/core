; Test: CNC Aluminum Machining Optimization
; Tests S-curve settings optimized for aluminum machining
; Requires: Aluminum test block, appropriate end mill, proper speeds/feeds

; SAFETY: User must configure spindle speeds and feeds for their setup
; This test pattern focuses on motion optimization, not cutting parameters

G28                 ; Home all axes
G54 G90 G94         ; Work coordinate system, absolute positioning

; Aluminum-optimized S-curve settings
M204 P10000 T5000   ; Aggressive acceleration for aluminum
M205 X500 Z250      ; High jerk values for responsive motion
M206 M1.4 C0.8 A1   ; Aggressive motion parameters
M210 F1.5 J1.3 A90  ; Fast junction optimization
M211 S1 P0.02 R3.0 V75 F0.8 L10  ; Optimized path blending
M212 V60.0 Q3.0 D2.0 ; Quick positioning for aluminum

M207                ; Report current settings

; Test 1: High-speed roughing simulation
; Aluminum allows high feed rates and aggressive motion
G1 F1000 X20 Y20    ; Position for test
G1 F3000 X80 Y20    ; High-speed traverse
G1 Y25 F600         ; Plunge rate
G1 X20 F3000        ; High-speed return
G1 Y30 F600         ; Next level
G1 X80 F3000        ; Traverse
G1 Y35 F600
G1 X20 F3000
G1 Y40 F600
G1 X80 F3000
G1 Y45 F600
G1 X20 F3000

; Test 2: Aluminum finishing characteristics
; Test surface finish with aggressive S-curve settings
G1 F2000 X20 Y45    ; Position for finishing
G1 X80 Y45          ; Finishing pass
G1 Y20              ; Side pass
G1 X20              ; Complete boundary
G1 Y45              ; Return

; Test 3: Complex aluminum profiling
; Aluminum can handle complex geometry at high speeds
G1 F2500 X30 Y25    ; Start complex profile
G1 X35 Y35          ; Ramp up
G1 X45 Y42          ; Curve
G1 X55 Y45          ; Peak
G1 X65 Y42          ; Curve down
G1 X75 Y35          ; Ramp down
G1 X80 Y25          ; End point
G1 X75 Y15          ; Return curve
G1 X65 Y8           ; Low point
G1 X55 Y5           ; Minimum
G1 X45 Y8           ; Return curve
G1 X35 Y15          ; Ramp up
G1 X30 Y25          ; Close profile

; Test 4: Aluminum pocket clearing
; High-speed pocket clearing with rapid direction changes
G1 F2500 X25 Y50    ; Position for pocket
; Aggressive zigzag pattern possible with aluminum
G1 X30 Y55
G1 Y65
G1 X35
G1 Y55
G1 X40
G1 Y65
G1 X45
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
G1 X75
G1 Y55

; Test 5: High-speed contours
; Aluminum allows very aggressive contouring
G1 F4000 X50 Y30    ; High-speed positioning
; Rapid contour following
G1 X52 Y33
G1 X56 Y36
G1 X62 Y38
G1 X68 Y39
G1 X74 Y38
G1 X78 Y36
G1 X80 Y33
G1 X81 Y30
G1 X80 Y27
G1 X78 Y24
G1 X74 Y22
G1 X68 Y21
G1 X62 Y22
G1 X56 Y24
G1 X52 Y27
G1 X50 Y30

; Test 6: Corner sharpness test
; Aluminum can maintain sharp corners at higher speeds
G1 F3000 X40 Y40    ; Position for corner test
G1 X60 Y40          ; Sharp 90-degree corner
G1 X60 Y60          ; Another sharp corner
G1 X40 Y60          ; Sharp corner
G1 X40 Y40          ; Complete square with sharp corners

; Performance evaluation:
; - Monitor for smooth high-speed motion without vibration
; - Check corner sharpness and accuracy at aggressive settings
; - Validate rapid direction changes maintain position accuracy
; - Confirm cycle time improvements vs conservative settings
; - Assess readiness for aluminum production machining

G28                 ; Return home