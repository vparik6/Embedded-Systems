; Assembly file for increment() function
; Lab 3, ECE 266, fall 2017
; Created by Zhao Zhang

; Function prototype: stopWatchIncrement(displayState_t *pDisplayState);
; This function increment the number shown on the stop watch display. It should be called once every one centisecond.
					.text
					.global stopWatchIncrement	; make this symbol global
stopWatchIncrement	.asmfunc

					; Update c2, continue to c1 if c2 is reset to zero
					LDRB	r1, [r0, #0]			; load c2 to r1, note c2's offset is zero which can be omitted
					ADD		r1, #1				; increment c2 by one
					STRB	r1, [r0, #0]			; store back c2
					CMP		r1, #10				; compare c2 and 10
					BNE		return				; if c2 != 10, jump to return
					MOV		r1, #0				; reset c2 to zero
					STRB	r1, [r0, #0]			; store back again


					; this is c1
					LDRB	r2, [r0, #1]			; load c1 to r2, note c2's offset is zero which can be omitted
					ADD		r2, #1				; increment c2 by one
					STRB	r2, [r0, #1]			; store back c2
					CMP		r2, #10				; compare c2 and 10
					BNE		return				; if c2 != 10, jump to return
					MOV		r2, #0				; reset c2 to zero
					STRB	r2, [r0, #1]			; store back again

					; this is s1
					LDRB	r3, [r0, #2]			; load c1 to r2, note c2's offset is zero which can be omitted
					ADD		r3, #1				; increment c2 by one
					STRB	r3, [r0, #2]			; store back c2
					CMP		r3, #10				; compare c2 and 10
					BNE		return				; if c2 != 10, jump to return
					MOV		r3, #0				; reset c2 to zero
					STRB	r3, [r0, #2]			; store back again

					; this is s2
					LDRB	r9, [r0, #3]			; load c1 to r2, note c2's offset is zero which can be omitted
					ADD		r9, #1				; increment c2 by one
					STRB	r9, [r0, #3]			; store back c2
					CMP		r9, #6				; compare c2 and 10
					BNE		return				; if c2 != 10, jump to return
					MOV		r9, #0				; reset c2 to zero
					STRB	r9, [r0, #3]			; store back again

					; YOUR CODE: update c1, s2, and s1


return				BX		LR                   ; return

					.endasmfunc

					.end
