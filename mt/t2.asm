mod_t2: begin
section text
    public mod_t2
    public v1
    public v2
    public ret2
    ret1: extern
    mod_t3: extern
    abs: extern
    input v1
    input v1 + 1
    input v1 + 2
    copy v1, v2 + 2
    copy v1 + 1, v2 + 1
    copy v1 + 2, v2
    jmp mod_t3
    ret2: store abs
    jmp ret1
section data
	v1: space 3
    v2: space 3
end