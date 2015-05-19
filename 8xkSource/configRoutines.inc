;====== Equates ================================================================
; In theory, larger values lead to more accurate results.  Or not.  I donno.
numOfTicks		.equ	250		; 32
numOfIterations	.equ	200		; 99 ; 10

findDivisors:
; Uses OP1-OP5. OP5 stores # cycles per interrupt. OP4 stores least error from calculations.
; OP1-OP3 are for calculations
; Inputs:	OP1 = CPU speed
; 			OP2 = Desired interrupt frequency (calls/second)
; Outputs: 	hl = divisor
;			de = counter
; Destroys:		all registers and OP2-OP5
; Preserved:	OP1
	bcall(_PushRealO1)		; Save OP1 on Floating Point Stack (FPS)
	bcall(_OP4Set1)			; Set minimum error to 1 (more than greatest remainder)
	bcall(_FPDiv)
	bcall(_OP1ToOP5)		; Backup interrupt_cycles in OP5
	ld	hl, 256
	bcall(_SetXXXXOP2)
	bcall(_FPDiv)
	bcall(_ConvOP1)			; de = OP1 = min value of divisor
	ld 	bc, 0
	push	bc				; This stack entry is used to store the best divisor
	ex	de, hl
	;inc	hl					; hl = divisor
	;bit 0, l				
	;jp	Z, while			; If hl is odd
	;inc hl					;	Make it even! (CPU divisors must be even)
	
while:
	push 	hl
	bcall(_SetXXXXOP2)		; OP2 = divisor
	bcall(_OP5ToOP1)		; OP1 = interrupt_cycles
	bcall(_FPDiv)
	bcall(_OP1ToOP2)		; OP2 = OP1 = interrupt_cycles/divisor
	bcall(_Int)				; Round OP1 to nearest int
	bcall(_FPSub)			; OP1 = remainder error (distance from nearest int)
	bcall(_OP4ToOP2)		; OP2 = minimum error
	bcall(_AbsO1O2Cp)		; Compare Abs(remainder) to minimum error. Carry is set only if OP1 < OP2
	
	jr	NC, keepError		; If remainder is less than current minimum error
	bcall(_OP1ToOP4)		; 	Update new minimum (OP3)
	pop	hl					;	Get current divisor
	pop af					;	Pop off old best divisor
	push	hl				;	Store current as new best
	push	hl				
	
keepError:
	pop	hl
	inc	l
	inc l
	ld	a, l
	cp	$82					; hl == 130
	jr	NZ, while
	
;Find Divisor
	pop hl					; hl = best divisor
	push	hl
	bcall(_SetXXXXOP2)		; OP2 = best divisor
	bcall(_OP5ToOP1)		; OP1 = interrupt_cycles
	bcall(_FPDiv)
	bcall(_Int)				; Round OP1 to nearest int
	
	bcall(_ConvOP1)
	push	de
	
	bcall(_PopRealO1)
	pop	de
	pop	hl
	ret

	
findCPUSpeed:
; The following section calls the speedTest routine several times and averages
; the results.
	bcall(_OP1Set0)
	bcall(_OP1ToOP6)
	ld	hl, numOfIterations
	ld	de, 0
CPULoop:
	push	hl
	push	de
	call	speedTest
	bcall(_OP6ToOP2)
	bcall(_FPAdd)
	bcall(_OP1ToOP6)
	pop	de
	pop	hl
	dec	hl
	call	cphlde
	jr	nz, CPULoop
	
	bcall(_OP6ToOP1)
	ld	hl, numOfIterations
	bcall(_SetXXXXOP2)
	bcall(_FPDiv)
	bcall(_PushRealO1)		; Backup OP1
	ret


speedTest:
; This uses the crystal timers to time a loop which takes a known amount of CPU 
; clock cycles.  This is then used to calculate the CPU clock speed.  If you 
; make the loop longer or shorter, remember to update the section below that
; computes the speed or you might get some wild results!  The loop should be as
; short as possible for better results.
	di
	ld	hl, $FFFF
	ld	a, $44				; 32768 Hz
	out	($30), a
	xor	a
	out	($31), a
	ld	a, numOfTicks
	out	($32), a
	ld	b, %00100000
speedLoop:
	inc	hl					;  6
	in	a, (4)				; 11
	and b					;  4
	jp	Z, speedLoop		; 10
							; 31 cycles
	xor	a
	out	($31), a
	out	($31), a
	ei
; Compute speed
;	push	hl
;	bcall(_DispHL)
;	pop	hl
	push	hl
	ld	a, 1
	bcall(_SetXXOP1)
	ld	hl, 32768
	bcall(_SetXXXXOP2)
	bcall(_FPDiv)
	ld	hl, numOfTicks
	bcall(_SetXXXXOP2)
	bcall(_FPMult)
	bcall(_OP1ToOP4)
	ld	a, 31
	bcall(_SetXXOp1)
	pop	hl
	bcall(_SetXXXXOP2)
	bcall(_FPMult)
	bcall(_OP4ToOP2)
	bcall(_FPDiv)
	ret

	
cphlde:
; A classic.
	or	a
	sbc	hl,de
	add	hl,de
	ret