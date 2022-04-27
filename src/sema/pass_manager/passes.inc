

#ifndef PASS_TUPLE
    #define PASS_TUPLE(funcname)
#endif
#ifndef COMPOUND_PASS_TUPLE
    #define COMPOUND_PASS_TUPLE(funcname, ...)
#endif

PASS_TUPLE(silly_pass);
PASS_TUPLE(silly_pass2);
PASS_TUPLE(silly_pass3);

//COMPOUND_PASS_TUPLE(some_silly, silly_pass, silly_pass2)
//COMPOUND_PASS_TUPLE(all_silly, some_silly, silly_pass3)
COMPOUND_PASS_TUPLE(all_silly, silly_pass, silly_pass2, silly_pass3)


#undef PASS_TUPLE
#undef COMPOUND_PASS_TUPLE
