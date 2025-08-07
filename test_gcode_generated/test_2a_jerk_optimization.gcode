; Test 2A: Systematic Jerk Value Testing
; Tests jerk values from conservative to aggressive
; Generated for travel envelope: 300.0x300.0x150.0mm
; Usable range: X25-275.0, Y25-275.0, Z5-100

G28 G90 G94
M208                ; Reset to defaults

; Define test pattern (complex geometry to stress jerk limits)
; Move to start position
G1 F1000 X75 Y75

; Test 1: Very Conservative Jerk (NEMA17 equivalent)
M205 X150 Z80       ; Conservative baseline
M207                ; Report settings
; Complex test pattern using 150mm range
G1 F3000 X225 Y75    ; Fast linear move (150mm)
G1 X225 Y225            ; 90-degree corner (150mm)
G1 X75 Y225          ; Another corner (150mm)
G1 X75 Y150          ; Shorter move (75mm)
G1 X150 Y150            ; Short move with direction change (75mm)
G1 X150 Y75          ; Return to start (75mm)
G4 P3               ; Pause for evaluation

; Test 2: NEMA23 Conservative
M205 X250 Z125      ; Higher torque motor capability
M207                ; Report settings
; Same pattern
G1 F3000 X225 Y75
G1 X225 Y225
G1 X75 Y225
G1 X75 Y150
G1 X150 Y150
G1 X150 Y75
G4 P3

; Test 3: NEMA23 Moderate
M205 X350 Z175      ; Moderate settings
M207
G1 F3000 X225 Y75
G1 X225 Y225
G1 X75 Y225
G1 X75 Y150
G1 X150 Y150
G1 X150 Y75
G4 P3

; Test 4: NEMA23/24 Aggressive
M205 X500 Z250      ; Aggressive for high-torque steppers
M207
G1 F3000 X225 Y75
G1 X225 Y225
G1 X75 Y225
G1 X75 Y150
G1 X150 Y150
G1 X150 Y75
G4 P3

; Test 5: Servo System Aggressive
M205 X700 Z350      ; Servo motor capabilities
M207
G1 F3000 X225 Y75
G1 X225 Y225
G1 X75 Y225
G1 X75 Y150
G1 X150 Y150
G1 X150 Y75
G4 P3

G28

; Evaluation Criteria:
; - Record vibration level (1-10 scale) for each test
; - Note any missed steps or position errors
; - Measure cycle time for each test sequence
; - Assess motion smoothness subjectively
; - Find highest jerk values before problems appear
