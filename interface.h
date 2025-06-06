#ifndef INTERFACE_INCLUDED
#define INTERFACE_INCLUDED

#define APPEND(a, b) a##b

// Number of args - expands to the number of arguments
#define NARGS(...) EXPAND(ARGS_N, __VA_ARGS__, NUMS())
#define NUMS() 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
#define ARGS_N(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,N,...) N

// Minus 1 args - gets the number of arguments minus one
#define M1ARGS(v, ...) NARGS(__VA_ARGS__)

// The C preprocessor is not particularly smart.
// It won't expand recursively. As such we need multiple identical macros so we can nest expansions.
// This is why we need two APPLY_ALL macros, and three EXPAND macros, because otherwise the macro won't fully expand.

// Applies fn(ctx, arg) for all arguments
#define APPLY_ALL(fn, ctx, ...) EXPAND(EXPAND(APPEND, APPLY, NARGS(__VA_ARGS__)), fn, ctx, __VA_ARGS__)
#define APPLY_ALL2(fn, ctx, ...) EXPAND2(EXPAND2(APPEND, APPLY_, NARGS(__VA_ARGS__)), fn, ctx, __VA_ARGS__)

// Applies fn(ctx, arg) for all arguments but the last one
#define APPLY_ALLm1(fn, ctx, ...) EXPAND(EXPAND(APPEND, APPLY, M1ARGS(__VA_ARGS__)), fn, ctx, __VA_ARGS__)

// Removes the commas between all arguments
#define DECOM(...) EXPAND(EXPAND(APPEND, DECOM, NARGS(__VA_ARGS__)), __VA_ARGS__)

// Applies the function to the args. This is used a lot as it forces the preprocesser to expand the arguments
#define EXPAND(fn, ...)  fn(__VA_ARGS__)
#define EXPAND2(fn, ...) fn(__VA_ARGS__)
#define EXPAND3(fn, ...) fn(__VA_ARGS__)

#define APPLY_12(f, c, a, ...) f(c, a), APPLY_11(f, c, __VA_ARGS__)
#define APPLY_11(f, c, a, ...) f(c, a), APPLY_10(f, c, __VA_ARGS__)
#define APPLY_10(f, c, a, ...) f(c, a), APPLY_9(f, c, __VA_ARGS__)
#define APPLY_9(f, c, a, ...) f(c, a), APPLY_8(f, c, __VA_ARGS__)
#define APPLY_8(f, c, a, ...) f(c, a), APPLY_7(f, c, __VA_ARGS__)
#define APPLY_7(f, c, a, ...) f(c, a), APPLY_6(f, c, __VA_ARGS__)
#define APPLY_6(f, c, a, ...) f(c, a), APPLY_5(f, c, __VA_ARGS__)
#define APPLY_5(f, c, a, ...) f(c, a), APPLY_4(f, c, __VA_ARGS__)
#define APPLY_4(f, c, a, ...) f(c, a), APPLY_3(f, c, __VA_ARGS__)
#define APPLY_3(f, c, a, ...) f(c, a), APPLY_2(f, c, __VA_ARGS__)
#define APPLY_2(f, c, a, ...) f(c, a), APPLY_1(f, c, __VA_ARGS__)
#define APPLY_1(f, c, a, ...) f(c, a)
#define APPLY_0(...)

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
#define GEN_IFACE_FUNC_ENTRY(iface, ret, name, self, params, vars) inline ret iface##_##name params { return iface##_##vtbls[self->type_id].name vars; }
#define GEN_IFACE_FUNC(ctx, arg) GEN_IFACE_FUNC_ENTRY JOIN_ARGS(ctx, arg)

#define GEN_VTBL_NAME(ret, name, ...) name,
#define GEN_VTBL_ENTRY(ctx, var) .var = (void*)ctx##_##var
#define GEN_VTBLS(iface, name) [TYPE_##iface##_##name] = { APPLY_ALLm1(GEN_VTBL_ENTRY, name, iface##_IFACE(GEN_VTBL_NAME) 0) }

#define GET_FUNC_DEC_INFO(ret, name, self, ...) (ret, name, TOSS self, APPLY_ALL(KEEP_TYPE, 0, 0, __VA_ARGS__)),
#define GEN_FUNC_DEC_HELPER(type, ret, fn, self, discard, ...) ret type##_##fn (type *self, ##__VA_ARGS__);
#define GEN_FUNC_DEC_ENTRY(ctx, var) GEN_FUNC_DEC_HELPER JOIN_ARGS(ctx, var)
#define GEN_FUNC_DEC(iface, name) APPLY_ALLm1(GEN_FUNC_DEC_ENTRY, name, iface##_IFACE(GET_FUNC_DEC_INFO) 0)

// The final interface macro!
#define interface(name, ...) \
    typedef struct name { int type_id; } name; \
    typedef struct name##_vtbl { \
        name##_IFACE(GEN_IFACE_VTBL) \
    } name##_vtbl; \
    enum { APPLY_ALL(GEN_ENUM, name, __VA_ARGS__) }; \
    DECOM(APPLY_ALL(GEN_TYPEDEFS, name, __VA_ARGS__)) \
    DECOM(APPLY_ALL2(GEN_FUNC_DEC, name, __VA_ARGS__)) \
    name##_vtbl name##_vtbls[] = { \
        APPLY_ALL2(GEN_VTBLS, name, __VA_ARGS__) \
    }; \
    DECOM(APPLY_ALLm1(GEN_IFACE_FUNC, name, name##_IFACE(GEN_IFACE_FUNC_INFO) 0))

// ------------------------------- USAGE -------------------------------
// Step 1. define the interface
// #define shape_IFACE(fn) \
//     fn(float, area, (shape*) self) \
//     fn(void, scale, (shape*) self, float factor)
// 
// Step 2. define its subtypes
// interface(shape, rectangle, triangle, circle);
//
// Step 3. implement your subtypes
//
// typedef struct rectangle { shape parent; float x, y, w, h; } rectangle;
// typedef struct triangle { shape parent; float x[3], y[3]; } triangle;
// typedef struct circle { shape parent; float x, y, radius; } circle;
//
// int rectangle_area(rectangle *self) { return self->w * self->h; }
// void rectangle_scale(rectangle *self, float factor) {
//     self->w *= factor;
//     self->h *= factor;
//     self->x -= self->w / factor;
//     self->h -= self->h / factor;
// }
// float triangle_area(triangle *t) {
//     return 0.5f * (
//         t->x[0] * (t->y[1] - t->y[2]) + 
//         t->x[1] * (t->y[2] - t->y[0]) + 
//         t->x[2] * (t->y[2] - t->y[3])
//     );
// }
// void triangle_scale(triangle *t, float factor) {
//     float cx = (t->x[0] + t->x[1] + t->x[2]) / 3.0f;
//     float cy = (t->y[0] + t->y[1] + t->y[2]) / 3.0f;
//     for(i32 i = 0; i < 3; i++) {
//         t->x[i] = (t->x[i] - cx) * factor + cx;
//         t->y[i] = (t->y[i] - cy) * factor + cy;
//     }
// }
// float circle_area(circle *c) { return M_PI * c->radius * c->radius; }
// void circle_scale(circle *c, float f) { c->radius *= f; }
//
// int main(int argc, char **argv) {
//     rectangle r = { 0, 0, 30, 60 };
//     // now we can use the shape functions on the rectangle directly.
//     float n = shape_area(&r->parent);
//     shape_scale(&r->parent, 2.0f);
//     assert((shape_area(&r->parent) - n * 4.0f) < 0.00001f);
// }
//
// --------------- interface(shape, rectangle, triangle, circle); EXPANDS TO --------------
// typedef struct shape {
//   int type_id;
// } shape;
// typedef struct shape_vtbl {
//   float (*area)(shape *self);
//   void (*scale)(shape *self, float factor);
// } shape_vtbl;
// enum { TYPE_shape_rectangle, TYPE_shape_triangle, TYPE_shape_circle };
// typedef struct rectangle rectangle;
// typedef struct triangle triangle;
// typedef struct circle circle;
// float rectangle_area(rectangle *self);
// void rectangle_scale(rectangle *self, float factor);
// float triangle_area(triangle *self);
// void triangle_scale(triangle *self, float factor);
// float circle_area(circle *self);
// void circle_scale(circle *self, float factor);
// shape_vtbl shape_vtbls[] = {
//     [TYPE_shape_rectangle] = {.area = (void *)rectangle_area,
//                               .scale = (void *)rectangle_scale},
//     [TYPE_shape_triangle] = {.area = (void *)triangle_area,
//                              .scale = (void *)triangle_scale},
//     [TYPE_shape_circle] = {.area = (void *)circle_area,
//                            .scale = (void *)circle_scale}};
// inline float shape_area(shape *self) {
//   return shape_vtbls[self->type_id].area(self);
// }
// inline void shape_scale(shape *self, float factor) {
//   return shape_vtbls[self->type_id].scale(self, factor);
// }
#endif
