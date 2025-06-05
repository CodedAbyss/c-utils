
#define nums() 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
#define args_n(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,N,...) N
#define nargs(...) expand(args_n, __VA_ARGS__, nums())
#define m1args(v, ...) nargs(__VA_ARGS__)
#define expand(fn, ...) fn(__VA_ARGS__)
#define append(a, b) a##b

#define decom8(a,...) a decom7(__VA_ARGS__)
#define decom7(a,...) a decom6(__VA_ARGS__)
#define decom6(a,...) a decom5(__VA_ARGS__)
#define decom5(a,...) a decom4(__VA_ARGS__)
#define decom4(a,...) a decom3(__VA_ARGS__)
#define decom3(a,...) a decom2(__VA_ARGS__)
#define decom2(a,...) a decom1(__VA_ARGS__)
#define decom1(a,...) a

#define apply8(f, c, a, ...) f(c, a), apply7(f, c, __VA_ARGS__)
#define apply7(f, c, a, ...) f(c, a), apply6(f, c, __VA_ARGS__)
#define apply6(f, c, a, ...) f(c, a), apply5(f, c, __VA_ARGS__)
#define apply5(f, c, a, ...) f(c, a), apply4(f, c, __VA_ARGS__)
#define apply4(f, c, a, ...) f(c, a), apply3(f, c, __VA_ARGS__)
#define apply3(f, c, a, ...) f(c, a), apply2(f, c, __VA_ARGS__)
#define apply2(f, c, a, ...) f(c, a), apply1(f, c, __VA_ARGS__)
#define apply1(f, c, a, ...) f(c, a)
#define APPLY_ALL(fn, ctx, ...) expand(expand(append, apply, nargs(__VA_ARGS__)), fn, ctx, __VA_ARGS__)
#define APPLY_ALLm1(fn, ctx, ...) expand(expand(append, apply, m1args(__VA_ARGS__)), fn, ctx, __VA_ARGS__)
#define decom(...) expand(expand(append, decom, nargs(__VA_ARGS__)), __VA_ARGS__)

#define get_name(ret, name, ...) name
#define keep(n) n
#define toss(n)
#define keep_type(ctx, var) keep var
#define extract_var(ctx, var) toss var

#define INTERFACE_DECLARE(ret, name, ...) ret (*name)(APPLY_ALL(keep_type, 0, __VA_ARGS__));
#define GEN_ENUM(ctx, var) TYPE##_##ctx##_##var
#define iface(name, ...) \
    typedef struct name { int type_id; } name; \
    typedef struct name##_vtbl { \
        name##_IFACE(INTERFACE_DECLARE) \
    } name##_vtbl; \
    decom(APPLY_ALLm1(func2, name, name##_IFACE(func1) 0)) \
    enum { APPLY_ALL(GEN_ENUM, name, __VA_ARGS__) }; \
    name##_vtbl name##_vtbls[];

#define func0(...) (APPLY_ALL(keep_type, 0, __VA_ARGS__)), (APPLY_ALL(extract_var, 0, __VA_ARGS__))
//#define func1(ret, name, self, ...) (ret, name, toss self, (keep self, APPLY_ALL(keep_type, 0, __VA_ARGS__)), (toss self, APPLY_ALL(extract_var, 0, __VA_ARGS__))),
#define func1(ret, name, ...) (ret, name, toss decom1(__VA_ARGS__), func0(__VA_ARGS__)),
#define collect(iface, ret, name, self, params, vars) inline ret iface##_##name params { return iface##_##vtbls[self->type_id].name vars; }
#define collect1(arg) collect arg
#define split_arg(...) __VA_ARGS__
#define split_arg1(ctx, arg) (ctx, split_arg arg)
#define func2(ctx, arg) collect1(split_arg1(ctx, arg))
#define def2(name) decom(APPLY_ALLm1(func2, name, name##_IFACE(func1) 0))

#define shape_IFACE(fn) \
    fn(int, area, (shape*) self) \
    fn(int, perim, (shape*) self)

iface(shape, rect, triangle, circle);

#define impl_func(ctx, var) .var = ctx##_##var,
#define impl(iface, name) iface##_vtbl name##_vtbl = { APPLY_ALLm1(impl_func, name, iface##_IFACE(get_name) 0) }

//#define def(iface, name) iface##_vtbl name##_vtbl = { APPLY_ALLm1(impl_func, name, iface##_IFACE(get_name) 0) }
//def(shape, square);
