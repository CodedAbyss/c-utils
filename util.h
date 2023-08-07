#define UTIL_ITER
#define UTIL_JSON
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
#ifdef UTIL_ITER_IMPL
typedef struct iter_t {
    bool (*next)(iter_t*, void*);
    void (*free)(iter_t*);
} iter_t;
#else
typedef struct iter_t iter_t;
#endif
void iter_free(iter_t *i);
bool iter_next(iter_t *i, void *ptr);
void *iter_collect(iter_t *i, i32 *len);
void *iter_collect_struct(iter_t *i, i32 size, i32 *len);
iter_t *iter_filter_map(iter_t *i, void *filter_map_func);
#define foreach(iter, value) for(iter_t *__i = iter; __i; __i = (iter_free(__i), (void*)0)) while(iter_next(__i, value))
#endif

#ifdef UTIL_STR
#ifdef UTIL_ITER
typedef struct token_t {
    char *ptr;
    i32 len;
} token_t;
// iterates char
iter_t *str_iter(char *str);
// iterates token_t
iter_t *str_iter_tok(char *str, i32 (*is_delimeter)(char*), i32 (*is_symbol)(char*));
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

typedef struct json_entry_t {
    char *name;
    json_t *value;
} json_entry_t;

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
json_t *json_obj_get_or_add(json_t *j, char *name, json_t *new);
void json_obj_add(json_t *j, char *name, json_t *new);
json_t *json_obj_del(json_t *j, char *name);

json_t *json_new_arr();
json_t *json_arr_get(json_t *j, i32 i);
void json_arr_add(json_t *j, i32 i, json_t *new);
json_t *json_arr_del(json_t *j, i32 i);

void json_fprint(json_t *j, FILE *f, char *indent);

#ifdef UTIL_ITER
// iterates json_entry_t
iter_t *json_iter_obj(json_t *j);
// iterates json_t*
iter_t *json_iter_arr(json_t *j);
#endif
#endif
#endif