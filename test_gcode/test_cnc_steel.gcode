; Test: CNC Steel Machining Optimization
; Tests S-curve settings optimized for steel machining on 500x500x200mm envelope
; Requires: Steel test block, carbide end mill, proper speeds/feeds
; Workpiece area: 225x225 to 275x275 (50x50mm workpiece for steel testing)

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

; Test 1: Steel roughing simulation (smaller workpiece area)
; Conservative feeds and smooth motion for tool life
G1 F800 X225 Y225   ; Position for test (50x50mm steel workpiece)
G1 F1500 X275 Y225  ; Moderate traverse speed (50mm)
G1 Y230 F300        ; Slow plunge for steel (5mm stepover)
G1 X225 F1500       ; Return traverse
G1 Y235 F300        ; Next level
G1 X275 F1500       ; Traverse
G1 Y240 F300
G1 X225 F1500
G1 Y245 F300
G1 X275 F1500
G1 Y250 F300
G1 X225 F1500
G1 Y255 F300
G1 X275 F1500
G1 Y260 F300
G1 X225 F1500
G1 Y265 F300
G1 X275 F1500
G1 Y270 F300
G1 X225 F1500
G1 Y275 F300

; Test 2: Steel finishing requirements
; Focus on surface finish and dimensional accuracy
G1 F1000 X225 Y275  ; Position for finishing
G1 X275 Y275        ; Finishing pass (slower for steel)
G1 Y225             ; Side pass
G1 X225             ; Complete boundary
G1 Y275             ; Return

; Test 3: Steel profiling with vibration control using larger machine capability
; Smooth curves to minimize tool chatter
G1 F1200 X200 Y200  ; Start profile (using more machine area)
G1 X220 Y230        ; Gentle curve (anti-chatter)
G1 X250 Y250        ; Gradual progression
G1 X280 Y260        ; Smooth transition
G1 X310 Y250        ; Symmetrical curve
G1 X330 Y230        ; Continue smooth motion
G1 X340 Y200        ; End point
G1 X330 Y170        ; Return curve
G1 X310 Y150        ; Smooth motion
G1 X280 Y140        ; Bottom of profile
G1 X250 Y150        ; Return curve
G1 X220 Y170        ; Smooth transition
G1 X200 Y200        ; Close profile smoothly

; Test 4: Steel pocket clearing with chatter prevention
; Slower, more deliberate pocket clearing for steel
G1 F1000 X230 Y250  ; Position for pocket (within workpiece)
; Conservative pocket clearing pattern
G1 X235 Y255
G1 Y265
G1 X240
G1 Y255
G1 X245
G1 Y265
G1 X250
G1 Y255
G1 X255
G1 Y265
G1 X260
G1 Y255
G1 X265
G1 Y265
G1 X270
G1 Y255

; Test 5: Steel contouring with smooth motion using full capability
; Emphasis on smooth motion to prevent work hardening
G1 F800 X180 Y180   ; Conservative positioning (larger area)
; Smooth contour for steel characteristics
G1 X190 Y200
G1 X210 Y220
G1 X240 Y230
G1 X270 Y235
G1 X300 Y230
G1 X320 Y220
G1 X330 Y200
G1 X335 Y180
G1 X330 Y160
G1 X320 Y140
G1 X300 Y130
G1 X270 Y125
G1 X240 Y130
G1 X210 Y140
G1 X190 Y160
G1 X180 Y180

; Test 6: Corner handling for steel
; Test precision corners without overshoot
G1 F600 X240 Y240   ; Very conservative for corners
G1 X260 Y240        ; Precise corner (20mm)
G1 X260 Y260        ; Another precise corner
G1 X240 Y260        ; Maintain accuracy
G1 X240 Y240        ; Complete precise square

; Test 7: Steel-specific vibration test
; Rapid direction changes at conservative settings
G1 F400 X248 Y248   ; Very slow for steel (center of workpiece)
G1 X252 Y248        ; Small incremental moves (4mm)
G1 X252 Y252
G1 X248 Y252
G1 X248 Y248
G1 X252 Y248        ; Repeat pattern to test consistency
G1 X252 Y252
G1 X248 Y252
G1 X248 Y248

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