#ifndef INTERFACE_INCLUDED
#define INTERFACE_INCLUDED

#define APPEND(a, b) a##b

// Number of args - expands to the number of arguments
#define NARGS(...) EXPAND(ARGS_N, __VA_ARGS__, NUMS())
#define NUMS() 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
#define ARGS_N(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,N,...) N

// Minus 1 args - gets the number of arguments minus one
#define M1ARGS(v, ...) NARGS(__VA_ARGS__)

// The C preprocessor is not particularly smart.
// It won't expand recursively. As such we need multiple identical macros so we can nest expansions.
// This is why we need two APPLY_ALL macros, and three EXPAND macros, because otherwise the macro won't fully expand.

// Applies fn(ctx, arg) for all arguments
#define APPLY_ALL(fn, ctx, ...) EXPAND(EXPAND(APPEND, APPLY, NARGS(__VA_ARGS__)), fn, ctx, __VA_ARGS__)

// Applies fn(ctx, arg) for all arguments but the last one
#define APPLY_ALLm1(fn, ctx, ...) EXPAND(EXPAND(APPEND, APPLY, M1ARGS(__VA_ARGS__)), fn, ctx, __VA_ARGS__)

// Removes the commas between all arguments
#define DECOM(...) EXPAND(EXPAND(APPEND, DECOM, NARGS(__VA_ARGS__)), __VA_ARGS__)

// Applies the function to the args. This is used a lot as it forces the preprocesser to expand the arguments
#define EXPAND(fn, ...)  fn(__VA_ARGS__)

#define APPLY24(f, c, a, ...) f(c, a), APPLY23(f, c, __VA_ARGS__)
#define APPLY23(f, c, a, ...) f(c, a), APPLY22(f, c, __VA_ARGS__)
#define APPLY22(f, c, a, ...) f(c, a), APPLY21(f, c, __VA_ARGS__)
#define APPLY21(f, c, a, ...) f(c, a), APPLY20(f, c, __VA_ARGS__)
#define APPLY20(f, c, a, ...) f(c, a), APPLY19(f, c, __VA_ARGS__)
#define APPLY19(f, c, a, ...) f(c, a), APPLY18(f, c, __VA_ARGS__)
#define APPLY18(f, c, a, ...) f(c, a), APPLY17(f, c, __VA_ARGS__)
#define APPLY17(f, c, a, ...) f(c, a), APPLY16(f, c, __VA_ARGS__)
#define APPLY16(f, c, a, ...) f(c, a), APPLY15(f, c, __VA_ARGS__)
#define APPLY15(f, c, a, ...) f(c, a), APPLY14(f, c, __VA_ARGS__)
#define APPLY13(f, c, a, ...) f(c, a), APPLY12(f, c, __VA_ARGS__)
#define APPLY12(f, c, a, ...) f(c, a), APPLY11(f, c, __VA_ARGS__)
#define APPLY11(f, c, a, ...) f(c, a), APPLY10(f, c, __VA_ARGS__)
#define APPLY10(f, c, a, ...) f(c, a), APPLY9(f, c, __VA_ARGS__)
#define APPLY9(f, c, a, ...) f(c, a), APPLY8(f, c, __VA_ARGS__)
#define APPLY8(f, c, a, ...) f(c, a), APPLY7(f, c, __VA_ARGS__)
#define APPLY7(f, c, a, ...) f(c, a), APPLY6(f, c, __VA_ARGS__)
#define APPLY6(f, c, a, ...) f(c, a), APPLY5(f, c, __VA_ARGS__)
#define APPLY5(f, c, a, ...) f(c, a), APPLY4(f, c, __VA_ARGS__)
#define APPLY4(f, c, a, ...) f(c, a), APPLY3(f, c, __VA_ARGS__)
#define APPLY3(f, c, a, ...) f(c, a), APPLY2(f, c, __VA_ARGS__)
#define APPLY2(f, c, a, ...) f(c, a), APPLY1(f, c, __VA_ARGS__)
#define APPLY1(f, c, a, ...) f(c, a)
#define APPLY0(...)

#define DECOM24(a,...) a DECOM23(__VA_ARGS__)
#define DECOM23(a,...) a DECOM22(__VA_ARGS__)
#define DECOM22(a,...) a DECOM21(__VA_ARGS__)
#define DECOM21(a,...) a DECOM20(__VA_ARGS__)
#define DECOM20(a,...) a DECOM19(__VA_ARGS__)
#define DECOM19(a,...) a DECOM18(__VA_ARGS__)
#define DECOM18(a,...) a DECOM17(__VA_ARGS__)
#define DECOM17(a,...) a DECOM16(__VA_ARGS__)
#define DECOM16(a,...) a DECOM15(__VA_ARGS__)
#define DECOM15(a,...) a DECOM14(__VA_ARGS__)
#define DECOM14(a,...) a DECOM13(__VA_ARGS__)
#define DECOM13(a,...) a DECOM12(__VA_ARGS__)
#define DECOM12(a,...) a DECOM11(__VA_ARGS__)
#define DECOM11(a,...) a DECOM10(__VA_ARGS__)
#define DECOM10(a,...) a DECOM9(__VA_ARGS__)
#define DECOM9(a,...) a DECOM8(__VA_ARGS__)
#define DECOM8(a,...) a DECOM7(__VA_ARGS__)
#define DECOM7(a,...) a DECOM6(__VA_ARGS__)
#define DECOM6(a,...) a DECOM5(__VA_ARGS__)
#define DECOM5(a,...) a DECOM4(__VA_ARGS__)
#define DECOM4(a,...) a DECOM3(__VA_ARGS__)
#define DECOM3(a,...) a DECOM2(__VA_ARGS__)
#define DECOM2(a,...) a DECOM1(__VA_ARGS__)
#define DECOM1(a,...) a
#define DECOM0(...)

// this may look pointless, but it's quite useful.
// we store the arguments like "(type) var_name"
// KEEP (type) var_name => type var_name
// TOSS (type) var_name => var_name
#define KEEP(n) n
#define TOSS(n)
#define KEEP_TYPE(ctx, var) KEEP var
#define EXTRACT_VAR(ctx, var) TOSS var

#define GEN_IFACE_VTBL(ret, name, ...) ret (*name)(APPLY_ALL(KEEP_TYPE, 0, __VA_ARGS__));
#define GEN_ENUM(ctx, var) TYPE##_##ctx##_##var
#define GEN_TYPEDEFS(ctx, var) typedef struct var var;

#define COPY(...) __VA_ARGS__
#define JOIN_ARGS(ctx, arg) (ctx, COPY arg)

#define GEN_IFACE_FUNC_HELPER(...) (APPLY_ALL(KEEP_TYPE, 0, __VA_ARGS__)), (APPLY_ALL(EXTRACT_VAR, 0, __VA_ARGS__))
#define GEN_IFACE_FUNC_INFO(ret, name, ...) (ret, name, TOSS DECOM1(__VA_ARGS__), GEN_IFACE_FUNC_HELPER(__VA_ARGS__)),
#define GEN_IFACE_FUNC_ENTRY(iface, ret, name, self, params, vars) inline ret iface##_##name params { return self->vtbl->name vars; }
#define GEN_IFACE_FUNC(ctx, arg) GEN_IFACE_FUNC_ENTRY JOIN_ARGS(ctx, arg)

#define GEN_VTBL_NAME(ret, name, ...) name,
#define GEN_VTBL_ENTRY(ctx, var) .var = (void*)ctx##_##var

#define GET_FUNC_DEC_INFO(ret, name, self, ...) (ret, name, TOSS self, APPLY_ALL(KEEP_TYPE, 0, 0, __VA_ARGS__)),
#define GEN_FUNC_DEC_HELPER(type, ret, fn, self, discard, ...) static ret type##_##fn (type *self, ##__VA_ARGS__);
#define GEN_FUNC_DEC_ENTRY(ctx, var) GEN_FUNC_DEC_HELPER JOIN_ARGS(ctx, var)
#define GEN_FUNC_DEC(iface, name) DECOM(APPLY_ALLm1(GEN_FUNC_DEC_ENTRY, name, iface##_IFACE(GET_FUNC_DEC_INFO) 0))

#define interface(name) \
    typedef struct name { struct name##_vtbl *vtbl; } name; \
    typedef struct name##_vtbl { \
        name##_IFACE(GEN_IFACE_VTBL) \
    } name##_vtbl; \
    DECOM(APPLY_ALLm1(GEN_IFACE_FUNC, name, name##_IFACE(GEN_IFACE_FUNC_INFO) 0))
#endif

#define impl(iface, impl) \
    DECOM(APPLY_ALLm1(GEN_FUNC_DEC_ENTRY, impl, iface##_IFACE(GET_FUNC_DEC_INFO) 0)) \
    iface##_vtbl iface##_##impl = { \
        APPLY_ALLm1(GEN_VTBL_ENTRY, impl, iface##_IFACE(GEN_VTBL_NAME) 0) \
    };

#endif
