mod3: begin
section data
    trash1: space
    trash2: const 20
section text
    hop3: extern
    public trash2
    public hop2
    hop2: jmp hop3
end

; codigo objeto esperado
; 0 20 5 0
; codigo do ligador esperado
; 5 6 13 9 14 0 5 10 0 20 5 2
; imagem de memoria esperada para 3 5 5 3 250 100 200
; 5 201 13 251 14 0 5 252 0 20 5 102 X