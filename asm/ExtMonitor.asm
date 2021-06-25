.OPT 

EXTROM				=	$8000	; EXTROM
PARPORT 			= 	$8800	; PARALLEL PORT $8800 (Hardware add-on)
SCREEN				=	$D000	; SCREEN RAM
ACIA				= 	$F000	; SERIAL PORT (MC6850 ACIA) FOR C1/Superboard

MCLOADFLG 			= 	$E0		; Zero Page $E0 MC Load Flag

LDFLAG  			= 	$0203	; Load Flag
SCRSPD  			= 	$0205	; Scroll speed 1
TPFlag  			= 	$024F	; Tape on/off Flag

SIZE				=	2		; SCREEN SIZE (0=1K 48x16) (1=2K 48x32) (2=3K 48x48)

*=	EXTROM
;================================================================================
; CLEARSCREEN: Clears Screen
;;================================================================================
CLEARSCREEN:		LDY	#0
					STY	$F9
					LDA	#>SCREEN
					STA	$FA
					LDX	#(SIZE + 1) * 4 ; 1k(4 * 246) 2k(8 * 256) 2k(12 * 256)
					LDA	#$20
_CLRLOOP:			STA	($F9),Y
					INY
					BNE	_CLRLOOP
					INC	$FA
					DEX
					BNE	_CLRLOOP
					RTS
;================================================================================
; FINTAPE - Turns off Tape related flags LOAD, SAVE, TPFL, MCLOAD.
;================================================================================
FINTAPE:			LDA #$00     
          			STA SCRSPD		; Scroll delay
          			STA LDFLAG		; Load Flag
          			STA MCLOADFLG	; Zero Page $E0
          			LDA #$11     
          			STA ACIA		; Relocated ACIA
          			STA TPFlag		; Tape on/off Flag
          			RTS  

;================================================================================
; [$FEEF] PRINTINIT - Initialize Printing 6520 at $8800-$8803 to operate as a parallel port with handshake.
;================================================================================
PRINTINIT:          LDA #$2A     
          			STA PARPORT + 1	; Parallel Port $8801
          			LDA #$FF     
          			STA PARPORT		; Parallel Port $8800
          			LDA #$2E     
          			STA PARPORT + 1	; Parallel Port $8801
          			LDA #$2C     
          			STA PARPORT + 3	; Parallel Port $8803
          			RTS          
;================================================================================
; [$FF04] PRINTOUT - Send Character to Printer Port Writes character in .A to PIA at $8800 configured as output port.
;================================================================================
; (PA0-PA7 = D0-D7) (CA2 = DS Active LO) (RBQ - off line/empty Active HI) (PB1 - ACK) (PB6 - Busy Active HI)
PRINTOUT:          	PHA          
          			LDA PARPORT + 1	; Parallel Port $8801
          			ORA #$04     
          			STA PARPORT + 1	; Parallel Port $8801
          			PLA          
          			STA PARPORT		; Parallel Port $8800
          			LDA PARPORT + 2	; Parallel Port $8802
          			LSR          
          			BCS _PRINTEXIT   
          			NOP          
          			NOP          
          			NOP          
_PRINTLOOP:    		BIT PARPORT + 2	; Parallel Port $8802
          			BMI _PRINTLOOP   
          			LDA PARPORT		; Parallel Port $8800
_PRINTEXIT:			RTS          

