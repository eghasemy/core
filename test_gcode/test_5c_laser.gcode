; Test 5C: Laser Cutting Test Pattern
; Simulates laser cutting operations

M208 G28 G90 G94
; Laser settings
M204 P6000 T3000
M205 X40 Z25
M206 M1.4 C0.8 A1

; Outside cut (fast)
G1 F3600 X10 Y10
G1 X90 Y10
G1 Y90
G1 X10
G1 Y10

; Detail cuts (slower)
G1 F1800 X30 Y30
G1 X70 Y30
G1 Y70
G1 X30
G1 Y30

; Intricate pattern
G1 F1200 X50 Y30
G1 X60 Y40
G1 X50 Y50
G1 X40 Y40
G1 X50 Y30

; Text simulation (small details)
G1 F900 X20 Y60
G1 Y80
G1 X25
G1 Y75
G1 X30 Y80
G1 Y60

G28