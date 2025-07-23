; Test file for S-curve final deceleration optimization
; This file tests the fix for "crawling to stop" issue in jog motions
; 
; This test compares different final deceleration settings
; and measures the time for the last portion of movement

; Initialize with known settings
G90          ; Absolute positioning
G21          ; Millimeter mode
M208         ; Reset S-curve to defaults
G28 X Y Z    ; Home all axes

; =====================================
; Test 1: Default settings (baseline)
; =====================================
M212 V30.0 Q2.0 D1.0  ; Default final deceleration settings

; Test jog motions at different speeds
$J=X10 F1000    ; Fast jog
$J=X0 F1000     ; Return fast
G4 P1           ; Pause 1 second

$J=Y10 F500     ; Medium speed jog  
$J=Y0 F500      ; Return medium
G4 P1

$J=Z5 F200      ; Slow jog
$J=Z0 F200      ; Return slow
G4 P2

; =====================================
; Test 2: Aggressive final deceleration
; =====================================
M212 V50.0 Q3.5 D2.0  ; More aggressive final deceleration

$J=X10 F1000    ; Fast jog with enhanced stopping
$J=X0 F1000     ; Return fast
G4 P1

$J=Y10 F500     ; Medium speed jog
$J=Y0 F500      ; Return medium  
G4 P1

$J=Z5 F200      ; Slow jog
$J=Z0 F200      ; Return slow
G4 P2

; =====================================
; Test 3: Conservative final deceleration
; =====================================
M212 V15.0 Q1.2 D0.5  ; More conservative final deceleration

$J=X10 F1000    ; Fast jog with gentle stopping
$J=X0 F1000     ; Return fast
G4 P1

$J=Y10 F500     ; Medium speed jog
$J=Y0 F500      ; Return medium
G4 P1

$J=Z5 F200      ; Slow jog  
$J=Z0 F200      ; Return slow
G4 P2

; =====================================
; Test 4: Mixed motion test
; =====================================
M212 V40.0 Q2.5 D1.5  ; Balanced settings

; Test combination of regular moves and jogs
G1 X5 Y5 F1000        ; Regular coordinated move
$J=X10 F800           ; Jog X
G1 Y10 F600           ; Regular Y move
$J=Y0 F800            ; Jog Y back
G1 X0 Y0 F1000        ; Regular return

; =====================================
; Test 5: Precision test
; =====================================
M212 V20.0 Q4.0 D0.8  ; High precision, fast final approach

; Very precise movements
$J=X1 F100            ; Slow, precise jog
$J=X0 F100            ; Return precise
G4 P1

$J=Y0.5 F50           ; Very slow, very precise
$J=Y0 F50             ; Return very precise
G4 P1

; =====================================
; Test 6: Speed variation test
; =====================================
M212 V35.0 Q2.8 D1.2  ; Good general-purpose settings

; Test various speeds to check consistency
$J=X5 F2000           ; Very fast
$J=X0 F2000
G4 P0.5

$J=Y5 F1000           ; Fast
$J=Y0 F1000  
G4 P0.5

$J=Z2 F500            ; Medium
$J=Z0 F500
G4 P0.5

$J=X2 F200            ; Slow
$J=X0 F200
G4 P0.5

$J=Y1 F100            ; Very slow
$J=Y0 F100
G4 P1

; =====================================
; Results reporting
; =====================================
M207         ; Report all S-curve settings
M212         ; Report final deceleration settings  
$$           ; Report all grbl settings

; Test complete - observe the differences in final approach behavior
; between different M212 settings. The goal is to eliminate
; "crawling" in the last 1-2mm of movement while maintaining
; smoothness and precision.

; Expected results:
; - Test 1 (defaults): May show some crawling behavior
; - Test 2 (aggressive): Should show minimal crawling, faster stops
; - Test 3 (conservative): Smooth but may still crawl slightly  
; - Tests 4-6: Demonstrate practical use cases

G28 X Y Z    ; Return to home position