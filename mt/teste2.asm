L1: EQU 1
L2:
    EQU 0

SECTION TEXT
MUL_N:
	MACRO
	MULT N
	STORE N
	END
DIV_N:
	MACRO
	DIV N
	STORE N
	END
IF L1
LOAD N
IF L2
INPUT N
IF L1
MUL_N
IF L2
DIV_N

input n
load n
_l1:
    add m
    jmpp _l1
    
copy m, n

SECTION DATA
N: SPACE
m: const 0x1

;arquivo de saida depois de passar as macros:
;SECTION TEXT
;LOAD N
;MULT N
;STORE N
;INPUT N
;LOAD N
;_L1: ADD M
;JMPP _L1
;COPY M, N
;SECTION DATA
;N: SPACE
;M: CONST 1

;codigo de maquina
;10 17 3 17 11 17 12 17 10 17 1 18 7 10 9 18 17 0 1
