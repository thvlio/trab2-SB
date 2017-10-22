L1: EQU 1
L2: 
    EQU 0
L3: EQU 1
L4:
    equ       l3
SECTION TEXT
IF L1
LOAD n;faz esta operação se L1 for verdadeiro
IF L2
    INPUT n;faz esta operação se L2 for verdadeiro
IF L3

SECTION DATA
N: SPACE

;arquivo do saida pre-processado:
;SECTION TEXT
;LOAD N
;SECTION DATA
;N: SPACE

;codigo de maquina
;10 2 0