mod2: begin
section data
    trash: space
section text
    hop2: extern
    public hop1
    hop1: jmp hop2
end

; codigo objeto esperado
; 0 5 0
; codigo do ligador esperado
; 5 6 13 9 14 0 5 10 0 20 5 2
; imagem de memoria esperada para 3 5 5 3 250 100 200
; 5 201 13 251 14 0 5 252 0 20 5 102 X