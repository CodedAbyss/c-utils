#define UTIL_TIME
#define UTIL_ITER
#define UTIL_JSON
#define UTIL_MAP
#define UTIL_STR

#ifndef UTIL_INCLUDED
#define UTIL_INCLUDED

#include <stdint.h>
#include <stdbool.h>

typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef float    f32;
typedef double   f64;

#ifdef UTIL_ITER
typedef struct iter_t iter_t;
void iter_free(iter_t *i);
bool iter_next(iter_t *i, void *ptr);
bool iter_next_pair(iter_t *i, void *ptr1, void *ptr2);

void *iter_collect(iter_t *i, i32 *len);
void *iter_collect_tiny(iter_t *i, i32 size, i32 *len);
iter_t *iter_filter_map(iter_t *i, void *filter_map_func);

#define __GET_FOREACH(_1, _2, FUNC, ...) FUNC
#define foreach(iter, ...) __GET_FOREACH(__VA_ARGS__, foreach2, foreach1)(iter, __VA_ARGS__)
#define foreach1(iter, value) for(iter_t *__i = iter; __i; __i = (iter_free(__i), (void*)0)) while(iter_next(__i, value))
#define foreach2(iter, key, value) for(iter_t *__i = iter; __i; __i = (iter_free(__i), (void*)0)) while(iter_next_pair(__i, key, value))
#endif

#ifdef UTIL_STR
bool  ch_is_an(char c);
bool  ch_is_ws(char c);
char *str_skip_ws(char *str);
char *str_fmt(char *fmt, i32 *size, ...);
char *str_ndup(char *str, i32 len);
char *str_dup(char *str);
char *str_add(char *str, char *extra, i32 *len);
i32   str_b_fmt(char *buf, i32 buf_size, char *fmt, ...);
void  str_b_dup(char *buf, i32 buf_size, char *str);
void  str_b_add(char *str, i32 buf_size, char *extra, i32 *len);
void  str_scanf(char *str, char *fmt, ...);
bool  str_to_f32(char *str, f32 *f);
bool  str_to_f64(char *str, f64 *f);
bool  str_to_i32(char *str, i32 *i);
bool  str_to_i64(char *str, i64 *i);
bool  str_to_bool(char *str, bool *b);
#ifdef UTIL_ITER
iter_t *str_iter(char *str);
iter_t *str_iter_tok(char *str, char *delims);
iter_t *str_iter_tok_ex(char *str, i32 (*is_delimeter)(char*), i32 (*is_symbol)(char*));
#endif
#endif

#ifdef UTIL_MAP
#define MAP_STRING map_str_hash, map_str_cmp, map_str_copy, map_str_free
#define MAP_ID map_id_hash, map_id_cmp, map_id_copy, map_id_free

typedef struct map_t map_t;

typedef u64 (*f_map_hash)(void*);
typedef i32 (*f_map_cmp)(void*, void*);
typedef void *(*f_map_copy_key)(void*);
typedef void (*f_map_free_key)(void*);

map_t *map_new(f_map_hash hash, f_map_cmp cmp, f_map_copy_key key_copy, f_map_free_key key_free);
void map_free(map_t *map);

i32 map_size(map_t *map);
i32 map_capacity(map_t *map);

void map_set(map_t *map, void *key, void *value);
void *map_get(map_t *map, void *key);
void *map_del(map_t *map, void *key);

u64 map_str_hash(void *key);
i32 map_str_cmp(void*, void*);
void *map_str_copy(void*);
void map_str_free(void*);

u64 map_id_hash(void *key);
i32 map_id_cmp(void*, void*);
void *map_id_copy(void*);
void map_id_free(void*);

#ifdef UTIL_ITER
iter_t *map_iter_pairs(map_t *map);
iter_t *map_iter_keys(map_t *map);
iter_t *map_iter(map_t *map);
#endif
#endif

#ifdef UTIL_JSON
#include <stdio.h>
enum {
    JSON_OBJECT,
    JSON_ARRAY,
    JSON_BOOL,
    JSON_NULL,
    JSON_FLOAT,
    JSON_INT,
    JSON_STRING
};

typedef struct json_t json_t;

//TODO: 
//Parse unicode
//Figure out what to do with null values

json_t *json_parse(char *buffer);
i32 json_type(json_t *j);
void json_set(json_t *j, json_t *new);
void json_free(json_t *j);

json_t *json_new_float(f32 f);
bool json_get_float(json_t *j, float *f);

json_t *json_new_int(i32 i);
bool json_get_int(json_t *j, i32 *i);

json_t *json_new_str(char *c);
bool json_get_str(json_t *j, char **ret);

json_t *json_new_obj();
json_t *json_obj_get(json_t *j, char *name);
void json_obj_add(json_t *j, char *name, json_t *new);
json_t *json_obj_del(json_t *j, char *name);

json_t *json_new_arr();
json_t *json_arr_get(json_t *j, i32 i);
void json_arr_add(json_t *j, i32 i, json_t *new);
json_t *json_arr_del(json_t *j, i32 i);

void json_fprint(json_t *j, FILE *f, char *indent);

#ifdef UTIL_ITER
iter_t *json_iter_obj(json_t *j);
iter_t *json_iter_arr(json_t *j);
#endif
#endif
#endif