; Test: CNC Aluminum Machining Optimization
; Tests S-curve settings optimized for aluminum machining on 500x500x200mm envelope
; Requires: Aluminum test block, appropriate end mill, proper speeds/feeds
; Workpiece area: 200x200 to 300x300 (100x100mm workpiece at machine center)

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
G1 F1000 X200 Y200  ; Position for test (100x100mm workpiece area)
G1 F3000 X300 Y200  ; High-speed traverse (100mm)
G1 Y210 F600        ; Plunge rate (10mm stepover)
G1 X200 F3000       ; High-speed return
G1 Y220 F600        ; Next level
G1 X300 F3000       ; Traverse
G1 Y230 F600
G1 X200 F3000
G1 Y240 F600
G1 X300 F3000
G1 Y250 F600
G1 X200 F3000
G1 Y260 F600
G1 X300 F3000
G1 Y270 F600
G1 X200 F3000
G1 Y280 F600
G1 X300 F3000
G1 Y290 F600
G1 X200 F3000
G1 Y300 F600

; Test 2: Aluminum finishing characteristics
; Test surface finish with aggressive S-curve settings
G1 F2000 X200 Y300  ; Position for finishing
G1 X300 Y300        ; Finishing pass (100mm)
G1 Y200             ; Side pass (100mm)
G1 X200             ; Complete boundary (100mm)
G1 Y300             ; Return (100mm)

; Test 3: Complex aluminum profiling
; Aluminum can handle complex geometry at high speeds
G1 F2500 X220 Y220  ; Start complex profile
G1 X230 Y240        ; Ramp up
G1 X250 Y260        ; Curve
G1 X270 Y270        ; Peak
G1 X290 Y260        ; Curve down
G1 X310 Y240        ; Ramp down (outside workpiece for testing)
G1 X300 Y220        ; End point
G1 X290 Y200        ; Return curve
G1 X270 Y190        ; Low point
G1 X250 Y185        ; Minimum
G1 X230 Y190        ; Return curve
G1 X220 Y200        ; Ramp up
G1 X220 Y220        ; Close profile

; Test 4: Aluminum pocket clearing
; High-speed pocket clearing with rapid direction changes
G1 F2500 X210 Y250  ; Position for pocket (within workpiece)
; Aggressive zigzag pattern possible with aluminum
G1 X220 Y260
G1 Y280
G1 X230
G1 Y260
G1 X240
G1 Y280
G1 X250
G1 Y260
G1 X260
G1 Y280
G1 X270
G1 Y260
G1 X280
G1 Y280
G1 X290
G1 Y260

; Test 5: High-speed contours using full travel capability
; Aluminum allows very aggressive contouring
G1 F4000 X150 Y150  ; High-speed positioning to larger area
; Rapid contour following across wider range
G1 X170 Y180
G1 X200 Y210
G1 X240 Y230
G1 X280 Y240
G1 X320 Y230
G1 X350 Y210
G1 X370 Y180
G1 X380 Y150
G1 X370 Y120
G1 X350 Y90
G1 X320 Y70
G1 X280 Y60
G1 X240 Y70
G1 X200 Y90
G1 X170 Y120
G1 X150 Y150

; Test 6: Corner sharpness test using larger scale
; Aluminum can maintain sharp corners at higher speeds
G1 F3000 X175 Y175  ; Position for corner test
G1 X275 Y175        ; Sharp 90-degree corner (100mm)
G1 X275 Y275        ; Another sharp corner (100mm)
G1 X175 Y275        ; Sharp corner (100mm)
G1 X175 Y175        ; Complete square with sharp corners (100mm)

; Performance evaluation:
; - Monitor for smooth high-speed motion without vibration
; - Check corner sharpness and accuracy at aggressive settings
; - Validate rapid direction changes maintain position accuracy
; - Confirm cycle time improvements vs conservative settings
; - Assess readiness for aluminum production machining

G28                 ; Return home