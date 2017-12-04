mod1: begin
section text
    hop1: extern
    trash2: extern
    public hop3
    jmp hop1
    hop3: output trash2
    stop
end

; codigo objeto esperado
; 5 0 13 0 14
; codigo do ligador esperado
; 5 6 13 9 14 0 5 10 0 20 5 2
; imagem de memoria esperada para 3 5 5 3 250 100 200
; 5 201 13 251 14 0 5 252 0 20 5 102 X