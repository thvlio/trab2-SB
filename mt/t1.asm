mod_t1: begin
section text
    public abs
    public ret1
    mod_t2: extern
    jmp mod_t2
    ret1: output abs
    stop
section data
    abs: space
end
    