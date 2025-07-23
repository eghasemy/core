; Test 5A: Aluminum Machining Optimization
; Optimizes S-curve for aluminum cutting on 60x60mm workpiece
; Generated for travel envelope: 300.0x300.0x150.0mm
; Usable range: X25-275.0, Y25-275.0, Z5-100

; SETUP REQUIRED: 6061 aluminum, appropriate end mill, speeds/feeds
; Workpiece positioned at machine center: X150 Y150
; Test area: 60mm x 60mm

G28 G54 G90 G94
; M3 S18000         ; High speed for aluminum (ADJUST)

; Aluminum allows aggressive settings due to:
; - Easy machining characteristics
; - Good heat dissipation  
; - Minimal tool chatter concerns

; Test 1: Aggressive aluminum settings
M205 X500 Z250      ; High jerk for aluminum
M206 P1.4 Q0.8 S1   ; Aggressive motion
M210 F1.5 J1.3 A90  ; Fast cornering
M211 S1 P0.02 R3.0 V75 F0.8 L10  ; Optimized blending
M212 V60.0 Q3.0 D2.0 ; Quick positioning

G0 X120 Y120
G0 Z2
G1 Z-2.0 F400       ; Deeper cut possible in aluminum

; High-speed aluminum roughing simulation
G1 X180 Y120 F1200     ; High feed rate (60mm)
G1 Y132 F300 ; Step over
G1 X120 F1200                  ; Rapid traverse
G1 Y144 F300
G1 X180 F1200
G1 Y156 F300
G1 X120 F1200
G1 Y168 F300
G1 X180 F1200

; Aluminum finishing pass
G1 X180 Y180 F800        ; Finishing feed
G1 Y120
G1 X120
G1 Y180

G1 Z5
M5
G28

; Aluminum-Specific Metrics:
; - Surface finish quality at high feed rates
; - Corner sharpness with aggressive settings
; - Cycle time improvements vs quality
; - Tool life with optimized motion
; - Chip evacuation effectiveness
