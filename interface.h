
enum {
    SHAPE_SQUARE,
    SHAPE_CIRCLE
};
typedef struct shape {
    int type_id;
} shape;
typedef struct shape_vtbl {
    int (*get_area)(struct shape *);
    int (*get_perim)(struct shape *);
} shape_vtbl;

int square_area(shape *self) {
    return 0;
}
int square_perim(shape *self) {
    return 0;
}

int circle_area(shape *self) {
    return 0;
}
int circle_perim(shape *self) {
    return 0;
}

shape_vtbl shape_vtbls[] = {
    [SHAPE_SQUARE] = { .get_area = square_area, .get_perim = square_perim },
    [SHAPE_CIRCLE] = { .get_area = circle_area, .get_perim = circle_perim }
};

int shape_area(shape *s) { return shape_vtbls[s->type_id].get_area(s); }

//
// #define gen5(name, fn, ...) .fn = name##_##fn, gen4(name, __VA_ARGS__)
// #define gen4(name, fn, ...) .fn = name##_##fn, gen3(name, __VA_ARGS__)
// #define gen3(name, fn, ...) .fn = name##_##fn, gen2(name, __VA_ARGS__)
// #define gen2(name, fn, ...) .fn = name##_##fn
// #define show(...) __VA_ARGS__
//
// #define get_name(ret, name, ...) name, 
// #define gen(name, ...) expand(append, gen, nargs(__VA_ARGS__))
// //#define impl(iface, name) iface##_vtbl name##_vtbl = { expand(gen(name, iface##_IFACE(get_name) 0), name, iface##_IFACE(get_name) 0) }
// #define expand_to_impl(pre, ret, name, ...) .name = pre##_##name,
// #define impl(iface, name) iface##_vtbl name##_vtbl = { iface##_IFACE(expand_to_impl, name) }
//
// #define expand_to_fnptr(pre, ret, name, ...) ret (*name)(__VA_ARGS__) { return };
// #define expand_to_def(pre, ret, name, ...) ret name(__VA_ARGS__) { return pre##_vtbls[0].name(__VA_ARGS__); }
// #define interface(name) \
//     typedef struct name { int type_id; } name; \
//     typedef struct name##_vtbl { \
//         name##_IFACE(expand_to_fnptr, 0) \
//     } name##_vtbl; \
//     name##_vtbl name##_vtbls[]; \
//     name##_IFACE(expand_to_def, name) \


#define nums() 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
#define args_n(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,N,...) N
#define nargs(...) expand(args_n, __VA_ARGS__, nums())
#define m1args(v, ...) nargs(__VA_ARGS__)
#define expand(fn, ...) fn(__VA_ARGS__)
#define append(a, b) a##b

#define apply8(fn, ctx, arg, ...) fn(ctx, arg), apply7(fn, ctx, __VA_ARGS__)
#define apply7(fn, ctx, arg, ...) fn(ctx, arg), apply6(fn, ctx, __VA_ARGS__)
#define apply6(fn, ctx, arg, ...) fn(ctx, arg), apply5(fn, ctx, __VA_ARGS__)
#define apply5(fn, ctx, arg, ...) fn(ctx, arg), apply4(fn, ctx, __VA_ARGS__)
#define apply4(fn, ctx, arg, ...) fn(ctx, arg), apply3(fn, ctx, __VA_ARGS__)
#define apply3(fn, ctx, arg, ...) fn(ctx, arg), apply2(fn, ctx, __VA_ARGS__)
#define apply2(fn, ctx, arg, ...) fn(ctx, arg), apply1(fn, ctx, __VA_ARGS__)
#define apply1(fn, ctx, arg, ...) fn(ctx, arg)
#define apply_to_all(fn, ctx, ...) expand(expand(append, apply, nargs(__VA_ARGS__)), fn, ctx, __VA_ARGS__)
#define apply_to_allm1(fn, ctx, ...) expand(expand(append, apply, m1args(__VA_ARGS__)), fn, ctx, __VA_ARGS__)

#define shape2_IFACE(fn) \
    fn(int, area, (shape*) self, (int) ctx) \
    fn(int, perim, (shape*) self, (int) ctx)

#define get_name(ret, name, ...) name,
#define keep(n) n
#define toss(n)
#define keep_type(ctx, var) keep var
#define extract_var(ctx, var) toss var
#define gen_func(ret, name, ...) ret name(apply_to_all(keep_type, 0, __VA_ARGS__)) { return name##_vtbl(apply_to_all(extract_var, 0, __VA_ARGS__)); }

//#define collect_declarations(

#define impl_func(ctx, var) .var = ctx##_##var
#define impl(iface, name) iface##_vtbl name##_vtbl = { apply_to_allm1(impl_func, name, iface##_IFACE(get_name) 0) }

impl(shape2, square);

#define def(iface, name) iface##_vtbl name##_vtbl = { apply_to_allm1(impl_func, name, iface##_IFACE(get_name) 0) }

def(shape2, square);


//get_vars(int, area, (shape*) self, (int) ctx)

//shape2_IFACE(gen_func)

//interface(shape2);
//impl(shape2, square);


// interface(shape2)
//
// shape2_vtbl shape2_vtbls[] = {
//     [SHAPE_SQUARE] = impl(shape2, square)
// }
