	TRIANGULO: ; comentario1
        EQU 1
SECTION DATA ; comentario2
	B:		SPACE
	H:		SPACE
	R:		SPACE
	DOIS:	CONST		2
SECTION TEXT
		INPUT		B
		INPUT		H
		LOAD		B
		MULT		H
		IF TRIANGULO ; comentario de novo
		DIV		DOIS
		STORE		R
		OUTPUT	R
		STOP

