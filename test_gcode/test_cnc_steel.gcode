; Test: CNC Steel Machining Optimization
; Tests S-curve settings optimized for steel machining
; Requires: Steel test block, carbide end mill, proper speeds/feeds

; SAFETY: User must configure spindle speeds and feeds for their setup
; Steel requires conservative settings to prevent tool chatter and extend tool life

G28                 ; Home all axes
G54 G90 G94         ; Work coordinate system, absolute positioning

; Steel-optimized S-curve settings (conservative)
M204 P6000 T3000    ; Moderate acceleration to reduce cutting forces
M205 X300 Z150      ; Conservative jerk to prevent chatter
M206 M1.1 C0.6 A1   ; Smooth motion priority
M210 F1.2 J1.0 A120 ; Gentle junction handling
M211 S1 P0.015 R2.0 V40 F0.6 L8  ; Careful path blending
M212 V35.0 Q2.2 D1.2 ; Controlled positioning

M207                ; Report current settings

; Test 1: Steel roughing simulation
; Conservative feeds and smooth motion for tool life
G1 F800 X20 Y20     ; Position for test
G1 F1500 X60 Y20    ; Moderate traverse speed
G1 Y25 F300         ; Slow plunge for steel
G1 X20 F1500        ; Return traverse
G1 Y30 F300         ; Next level
G1 X60 F1500        ; Traverse
G1 Y35 F300
G1 X20 F1500
G1 Y40 F300
G1 X60 F1500

; Test 2: Steel finishing requirements
; Focus on surface finish and dimensional accuracy
G1 F1000 X20 Y40    ; Position for finishing
G1 X60 Y40          ; Finishing pass (slower for steel)
G1 Y20              ; Side pass
G1 X20              ; Complete boundary
G1 Y40              ; Return

; Test 3: Steel profiling with vibration control
; Smooth curves to minimize tool chatter
G1 F1200 X25 Y25    ; Start profile
G1 X30 Y32          ; Gentle curve (anti-chatter)
G1 X37 Y37          ; Gradual progression
G1 X45 Y40          ; Smooth transition
G1 X53 Y37          ; Symmetrical curve
G1 X60 Y32          ; Continue smooth motion
G1 X65 Y25          ; End point
G1 X60 Y18          ; Return curve
G1 X53 Y13          ; Smooth motion
G1 X45 Y10          ; Bottom of profile
G1 X37 Y13          ; Return curve
G1 X30 Y18          ; Smooth transition
G1 X25 Y25          ; Close profile smoothly

; Test 4: Steel pocket clearing with chatter prevention
; Slower, more deliberate pocket clearing for steel
G1 F1000 X30 Y50    ; Position for pocket
; Conservative pocket clearing pattern
G1 X32 Y52
G1 Y58
G1 X34
G1 Y52
G1 X36
G1 Y58
G1 X38
G1 Y52
G1 X40
G1 Y58
G1 X42
G1 Y52
G1 X44
G1 Y58
G1 X46
G1 Y52
G1 X48
G1 Y58
G1 X50
G1 Y52

; Test 5: Steel contouring with smooth motion
; Emphasis on smooth motion to prevent work hardening
G1 F800 X40 Y35     ; Conservative positioning
; Smooth contour for steel characteristics
G1 X41 Y37
G1 X43 Y38
G1 X46 Y39
G1 X49 Y39
G1 X52 Y38
G1 X54 Y37
G1 X55 Y35
G1 X54 Y33
G1 X52 Y32
G1 X49 Y31
G1 X46 Y31
G1 X43 Y32
G1 X41 Y33
G1 X40 Y35

; Test 6: Corner handling for steel
; Test precision corners without overshoot
G1 F600 X35 Y45     ; Very conservative for corners
G1 X45 Y45          ; Precise corner
G1 X45 Y55          ; Another precise corner
G1 X35 Y55          ; Maintain accuracy
G1 X35 Y45          ; Complete precise square

; Test 7: Steel-specific vibration test
; Rapid direction changes at conservative settings
G1 F400 X40 Y50     ; Very slow for steel
G1 X42 Y50          ; Small incremental moves
G1 X42 Y52
G1 X40 Y52
G1 X40 Y50
G1 X42 Y50          ; Repeat pattern to test consistency
G1 X42 Y52
G1 X40 Y52
G1 X40 Y50

; Performance evaluation for steel:
; - Monitor for absence of tool chatter and vibration
; - Check surface finish quality with conservative settings
; - Validate dimensional accuracy maintained
; - Confirm smooth motion without work hardening effects
; - Assess tool life preservation vs cycle time
; - Verify corner accuracy without overshoot

G28                 ; Return home

; Notes for steel machining:
; - Lower jerk values prevent shock loads on cutting edge
; - Smooth motion reduces work hardening tendency
; - Conservative corner handling prevents tool chipping
; - Consistent feed rates minimize heat generation
; - Path blending maintains smooth tool motion