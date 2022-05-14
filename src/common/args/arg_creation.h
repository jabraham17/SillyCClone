
#ifndef _TOOLS_HELPER_ARGS_ARG_CREATION_H_
#define _TOOLS_HELPER_ARGS_ARG_CREATION_H_

// empty str, no args just bool
// one colon, 1 required arg
// two colo, 1 optional arg
#define _WITH_ARG_str(_char, _variable) #_char ":"
#define _WITH_BOOL_str(_char, _variable) #_char ""
#define _WITH_CUSTOM_str(_char, _action_macro, _arg_options) #_char _arg_options

#define _WITH_ARG_act(_char, _variable)                                        \
    case #_char[0]:                                                            \
        _variable = optarg;                                                    \
        break;

#define _WITH_BOOL_act(_char, _variable)                                       \
    case #_char[0]:                                                            \
        _variable = 1;                                                         \
        break;

#define _WITH_CUSTOM_act(_char, _action_macro, _arg_options)                   \
    case #_char[0]: do { _action_macro;                                        \
        } while(0);                                                            \
        break;

#define MAKE_ARGS(_argc, _argv, _ARG_LIST)                                     \
    do {                                                                       \
        int _c;                                                                \
        opterr = 1;                                                            \
        while((_c = getopt(                                                    \
                   _argc, _argv,                                               \
                   _ARG_LIST(                                                  \
                       _WITH_ARG_str, _WITH_BOOL_str, _WITH_CUSTOM_str))) !=   \
              -1) {                                                            \
            switch(_c) {                                                       \
                _ARG_LIST(_WITH_ARG_act, _WITH_BOOL_act, _WITH_CUSTOM_act)     \
                case '?':                                                      \
                default: exit(1);                                              \
            }                                                                  \
        }                                                                      \
    } while(0)

// if i need to, extend this to include custom actions with either a macro arg
// or function arg, id prefer macro arg but thats just me
// _ARG_LIST(_WITH_ARG, _WITH_ARG, _WITH_BOOL)

#endif
