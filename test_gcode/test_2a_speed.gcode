; Test 2A: Speed Performance Test
; Measures time to complete complex path
; Designed for 500x500x200mm travel envelope

M208 G28 G90 G94
M205 X100 Z50       ; Baseline settings

; Record start time manually or use timer
; Complex path test using larger coordinates
G1 F4000 X50 Y50
G1 X400 Y50         ; 350mm move
G1 X400 Y400        ; 350mm move
G1 X50 Y400         ; 350mm move
G1 X50 Y50          ; 350mm move

; Circle approximation (octagon) - 200mm diameter
G1 X225 Y50
G1 X378.54 Y121.46   ; 8-sided polygon scaled up
G1 X400 Y225
G1 X378.54 Y328.54
G1 X225 Y400
G1 X121.46 Y328.54
G1 X50 Y225
G1 X121.46 Y121.46
G1 X225 Y50

; Zig-zag pattern across machine envelope
G1 X50 Y100
G1 X400 Y150        ; 350mm moves
G1 X50 Y200
G1 X400 Y250
G1 X50 Y300
G1 X400 Y350
G1 X50 Y400

G28
; Record end time