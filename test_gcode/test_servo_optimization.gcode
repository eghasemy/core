; Test: Servo Motor S-Curve Optimization
; Tests S-curve settings that take advantage of servo motor capabilities
; For JMC 180W DC servos, 400W Stepperonline A6 series, or similar closed-loop systems

; SAFETY: Verify servo tuning and following error limits before testing
; Servo systems can handle much more aggressive settings than steppers

G28                 ; Home all axes
G54 G90 G94         ; Work coordinate system, absolute positioning

; Servo-optimized S-curve settings (aggressive)
M204 P15000 T8000   ; Very aggressive acceleration (servo advantage)
M205 X700 Z350      ; High jerk values for servo responsiveness
M206 P1.5 Q0.8 S1   ; Take full advantage of servo precision
M210 F1.5 J1.4 A80  ; Aggressive junction optimization
M211 S1 P0.01 R3.0 V100 F0.8 L16  ; Advanced path blending
M212 V70.0 Q3.5 D2.0 ; Aggressive final positioning

M207                ; Report current settings

; Test 1: Servo high-speed positioning
; Test servo's ability to handle rapid acceleration changes
G1 F1000 X25 Y25    ; Initial positioning
G1 F6000 X75 Y25    ; Very high speed traverse
G1 Y75 F6000        ; High-speed direction change
G1 X25 F6000        ; Another high-speed move
G1 Y25 F6000        ; Complete high-speed square
; Monitor for following errors and position accuracy

; Test 2: Servo precision at speed
; Test maintaining accuracy during aggressive motion
G1 F5000 X30 Y30    ; High-speed positioning
G1 X70 Y30          ; 40mm precise move at high speed
G1 Y70              ; Precise corner at speed
G1 X30              ; Return move
G1 Y30              ; Complete precise square at high speed

; Test 3: Rapid direction changes (servo advantage)
; Test servo's ability to handle rapid reversals
G1 F4000 X50 Y30    ; Position for reversals
G1 X55 Y30          ; Quick move
G1 X45 Y30          ; Rapid reversal
G1 X55 Y30          ; Another reversal
G1 X45 Y30          ; Test servo response time
G1 X55 Y30
G1 X45 Y30
G1 X50 Y30          ; Return to center

; Test 4: Complex servo path following
; High-speed complex geometry only possible with servos
G1 F5000 X25 Y40    ; Start complex path
G1 X27 Y45          ; Small incremental moves at high speed
G1 X30 Y49
G1 X34 Y52
G1 X39 Y54
G1 X44 Y55
G1 X49 Y54
G1 X54 Y52
G1 X58 Y49
G1 X61 Y45
G1 X63 Y40
G1 X61 Y35
G1 X58 Y31
G1 X54 Y28
G1 X49 Y26
G1 X44 Y25
G1 X39 Y26
G1 X34 Y28
G1 X30 Y31
G1 X27 Y35
G1 X25 Y40

; Test 5: Servo micro-positioning at speed
; Test servo's ability to maintain accuracy in small moves
G1 F3000 X50 Y50    ; Center position
G1 X50.1 Y50        ; 0.1mm move at high feed
G1 X50 Y50.1        ; Another micro move
G1 X49.9 Y50        ; Reverse micro move
G1 X50 Y49.9        ; Test servo resolution
G1 X50.2 Y50        ; Slightly larger move
G1 X50 Y50.2
G1 X49.8 Y50
G1 X50 Y49.8
G1 X50 Y50          ; Return to center

; Test 6: Servo junction optimization
; Test advanced junction handling at high speeds
G1 F4000 X30 Y60    ; Start junction test
G1 X40 Y70          ; 45-degree junction
G1 X60 Y70          ; Sharp 90-degree junction
G1 X70 Y60          ; Another 45-degree junction
G1 X70 Y40          ; 90-degree junction
G1 X60 Y30          ; 45-degree junction
G1 X40 Y30          ; Sharp junction
G1 X30 Y40          ; 45-degree junction
G1 X30 Y60          ; Complete junction test

; Test 7: Servo path blending validation
; Test advanced path blending capabilities
G1 F3500 X35 Y35    ; Start blending test
; Series of connected moves that should blend smoothly
G1 X38 Y42
G1 X43 Y47
G1 X49 Y50
G1 X56 Y51
G1 X62 Y50
G1 X67 Y47
G1 X70 Y42
G1 X71 Y35
G1 X70 Y28
G1 X67 Y23
G1 X62 Y20
G1 X56 Y19
G1 X49 Y20
G1 X43 Y23
G1 X38 Y28
G1 X35 Y35

; Test 8: Servo acceleration curve testing
; Test servo's ability to handle S-curve acceleration profiles
G1 F2000 X40 Y40    ; Start acceleration test
G1 F6000 X60 Y40    ; Rapid acceleration to high speed
G1 F1000 Y60        ; Rapid deceleration 
G1 F6000 X40        ; Rapid acceleration again
G1 F1000 Y40        ; Rapid deceleration
; Test servo's ability to follow S-curve acceleration profiles

; Test 9: Servo stress test
; Push servo to maximum safe performance
G1 F7000 X30 Y30    ; Very high speed if servo capable
G1 X70 Y30
G1 Y70
G1 X30
G1 Y30
; Monitor servo temperature and following error

; Performance evaluation for servos:
; - Verify no following errors during high-speed motion
; - Check position accuracy maintained at aggressive settings
; - Confirm smooth motion without oscillation
; - Validate servo temperature remains within limits
; - Assess maximum usable jerk values
; - Document servo-specific optimal settings
; - Compare performance improvement vs stepper systems

G28                 ; Return home

; Servo optimization notes:
; - Closed-loop feedback allows much higher jerk values
; - Servo response time enables rapid direction changes
; - Position accuracy maintained even at high speeds
; - Advanced path blending possible with servo precision
; - Monitor following error and servo drive status
; - Adjust servo PID parameters if needed for optimal S-curve performance