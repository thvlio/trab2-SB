mod_t3: begin
section text
	v1: extern
    v2: extern
    ret2: extern
    load v1
    mult v2
    store vaux
    load v1 + 1 
    mult v2 + 1
    store vaux + 1
    load v1 + 2
    mult v2 + 2
    store vaux + 2
    load vaux
    mult vaux + 1
    mult vaux + 2
    jmp ret2
section data
	vaux: space 3
end