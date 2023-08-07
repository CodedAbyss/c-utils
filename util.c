#include "util.h"
#include <stdlib.h>
#include <string.h>

// Windows pseudo posix compatability
#ifdef _WIN32
#define strdup _strdup
#define sscanf sscanf_s
#endif

#ifdef UTIL_ITER
typedef struct iter_t {
    bool (*next)(iter_t*, void*);
    void (*free)(iter_t*);
} iter_t;
bool iter_next(iter_t *i, void *ptr) { return i->next(i, ptr); }

// sketchy iter functions
typedef struct filter_map_iter_t {
    iter_t iter;
    iter_t *inner;
    bool (*filter_map)(void*, void*);
} filter_map_iter_t;

bool __filter_map_iter_next(iter_t *i, void *ptr) {
    static char buf[2][64];
    static bool n = 0;
    n ^= 1;
    filter_map_iter_t *f_iter = (filter_map_iter_t*)i;
    bool success = false;
    while((success = f_iter->inner->next(f_iter->inner, buf[n])) && !f_iter->filter_map(buf[n], ptr));
    return success;
}

void __filter_map_iter_free(iter_t *i) {
    filter_map_iter_t *f_iter = (filter_map_iter_t*)i;
    iter_free(f_iter->inner);
    free(f_iter);
}

iter_t *iter_filter_map(iter_t *i, void *filter_map_func) {
    if(!i) return 0;
    filter_map_iter_t *f_iter = calloc(1, sizeof(filter_map_iter_t));
    f_iter->iter.free = __filter_map_iter_free;
    f_iter->iter.next = __filter_map_iter_next;
    f_iter->inner = i;
    f_iter->filter_map = (bool (*)(void*, void*))filter_map_func;
    return (iter_t*)f_iter;
}

void *iter_collect(iter_t *i, i32 *len) {
    if(!i) {
        *len = -1;
        return 0;
    }
    i32 size = 4, n = 0;
    void **arr = calloc(size, sizeof(void*));
    void *ptr;
    for(; iter_next(i, &ptr); n++) {
        if(n > 3 && !(n & (n - 1)))
            arr = realloc(arr, (size *= 2) * sizeof(void*));
        arr[n] = ptr;
    }
    *len = n;
    iter_free(i);
    return arr;
}

void *iter_collect_struct(iter_t *i, i32 size, i32 *len) {
    if(!i) {
        *len = -1;
        return 0;
    }
    i32 cap = 4, n = 0;
    char *tmp = malloc(size);
    char *arr = calloc(cap, size);
    for(; iter_next(i, tmp); n++) {
        if(n > 3 && !(n & (n - 1)))
            arr = realloc(arr, (cap *= 2) * size);
        memcpy(arr + n * size, tmp, size);
    }
    *len = n;
    free(tmp);
    iter_free(i);
    return arr;
}

void iter_free(iter_t *i) {
    if(i->free) i->free(i); // complex free
    else free(i); // basic free
}
#endif

#ifdef UTIL_STR
#ifdef UTIL_ITER
typedef struct str_iter_t {
    iter_t i;
    char *str;
} str_iter_t;
bool __str_iter_next(iter_t *i, void *ptr) {
    str_iter_t *s_iter = (str_iter_t*)i;
    i64 next = (i64)s_iter->str[0];
    if(!next) return 0;
    *(i64*)ptr = next;
    return 1;
}
iter_t *str_iter(char *str) {
    str_iter_t *ret = calloc(1, sizeof(str_iter_t));
    ret->i.next = __str_iter_next;
    ret->str = str;
    return (iter_t*)ret;
}
typedef struct str_iter_tok_t {
    iter_t i;
    i32 (*is_delimeter)(char*);
    i32 (*is_symbol)(char*);
    i32 sym;
    char *str;
} str_iter_tok_t;
bool __str_iter_tok_next(iter_t *i, void *ptr) {
    str_iter_tok_t *s_iter = (str_iter_tok_t*)i;
    i32 n = 0, l = 0;
    if(s_iter->sym) {
        *(token_t*)ptr = (token_t) { s_iter->str, s_iter->sym };
        s_iter->str += s_iter->sym;
        s_iter->sym = 0;
        return 1;
    }
    while((n = s_iter->is_delimeter(s_iter->str))) s_iter->str += n;
    for(n = 0; s_iter->str[n]; n++) {
        if((l = s_iter->is_delimeter(&s_iter->str[n]))) {
            *(token_t*)ptr = (token_t) { s_iter->str, n };
            s_iter->str += n + l;
            return 1;
        }
        if((l = s_iter->is_symbol(&s_iter->str[n]))) {
            if(n) {
                *(token_t*)ptr = (token_t) { s_iter->str, n };
                s_iter->str += n;
                s_iter->sym = l;
                return 1;
            } else {
                *(token_t*)ptr = (token_t) { s_iter->str, l };
                s_iter->str += n + l;
                return 1;
            }
        }
    }
    return 0;
}
iter_t *str_iter_tok(char *str, i32 (*is_delimeter)(char*), i32 (*is_symbol)(char*)) {
    str_iter_tok_t *ret = calloc(1, sizeof(str_iter_tok_t));
    ret->i.next = __str_iter_tok_next;
    ret->is_delimeter = is_delimeter;
    ret->is_symbol = is_symbol;
    ret->str = str;
    return (iter_t*)ret;
}
#endif
#endif

#ifdef UTIL_MAP
struct map_node_t {
    void *key;
    void *value;
    struct map_node_t *next;
};

typedef struct map_t {
    i32 size;
    i32 capacity;
    f_map_hash hash;
    f_map_cmp cmp;
    f_map_copy_key key_copy;
    f_map_free_key key_free;
    struct map_node_t **nodes;
} map_t;

// Forward declarations for private functions
struct map_node_t **__map_get_node(map_t *map, void *key);

map_t *map_new_sized(i32 capacity, f_map_hash hash, f_map_cmp cmp, f_map_copy_key key_copy, f_map_free_key key_free) {
    map_t *ret = calloc(1, sizeof(map_t));
    ret->hash = hash ? hash : map_str_hash;
    ret->cmp = cmp ? cmp : map_id_cmp;
    ret->key_copy = key_copy ? key_copy : map_id_copy;
    ret->key_free = key_free ? key_free : map_id_free;
    ret->capacity = capacity;
    ret->nodes = (struct map_node_t**)calloc(capacity, sizeof(struct map_node_t*));
    return ret;
}

map_t *map_new(f_map_hash hash, f_map_cmp cmp, f_map_copy_key key_copy, f_map_free_key key_free) {
    return map_new_sized(8, hash, cmp, key_copy, key_free);
}

void map_free(map_t *map) {
    struct map_node_t *t, *n;
    for(i32 i = 0; i < map->capacity; i++) {
        for(n = map->nodes[i]; n; n = t) {
            map->key_free(n->key);
            t = n->next;
            free(n);
        }
    }
    free(map->nodes);
    free(map);
}

i32 map_size(map_t *map) { return map->size; }
i32 map_capacity(map_t *map) { return map->capacity; }

struct map_node_t **__map_get_node(map_t *map, void *key) {
    i32 index = map->hash(key) % map->capacity;
    struct map_node_t **node = &map->nodes[index];
    while(*node && map->cmp((*node)->key, key))
        node = &(*node)->next;
    return node;
}

void map_set(map_t *map, void *key, void *value) {
    struct map_node_t *n, *p, **node = __map_get_node(map, key);
    if(!*node) {
        *node = calloc(1, sizeof(struct map_node_t));
        (*node)->key = map->key_copy(key);
    }
    (*node)->value = value;
    map->size++;
    if((float)map->size / map->capacity <= .75f) return;
    map_t *copy = map_new_sized(map->capacity * 2, map->hash, map->cmp, map_id_copy, map_id_free);
    for(i32 i = 0; i < map->capacity; i++) {
        for(n = map->nodes[i]; n; n = p) {
            map_set(copy, n->key, n->value);
            p = n->next;
            free(n);
        }
    }
    free(map->nodes);
    map->nodes = copy->nodes;
    map->capacity = copy->capacity;
    free(copy);
}

void *map_get(map_t *map, void *key){
    struct map_node_t **node = __map_get_node(map, key);
    if(!*node) return 0;
    return (*node)->value;
}

void *map_del(map_t *map, void *key) {
    struct map_node_t *n, **node = __map_get_node(map, key);
    if(!*node) return 0;
    void *ret = (*node)->value;
    if((*node)->next) {
        n = (*node)->next;
        **node = *n;
        free(n);
    } else {
        free(*node);
        *node = 0;
    }
    return ret;
}

// fnv1a
u64 map_str_hash(void *key) {
    char *s = (char*)key;
    u64 h = 0;
    while (*s != '\0')
	    h = (h ^ ((char)(*s++))) * 1099511628211u;
    return h;
}

i32 map_str_cmp(void *a, void *b) {
    char *sa = a, *sb = b;
    i32 i = 0;
    while(sa[i] && sb[i] && sa[i] == sb[i]) i++;
    return sa[i] - sb[i];
}

void *map_str_copy(void *p) {
    char *s = p;
    i32 l = 0;
    while(s[l]) l++;
    char *ret = malloc(l + 1);
    ret[l] = 0;
    for(i32 i = 0; i < l; i++)
        ret[i] = s[i];
    return ret;
}

void map_str_free(void *p) { free(p); }

u64 map_id_hash(void *key) { return (u64)key; }
i32 map_id_cmp(void *a, void *b) { return (i32)((char*)a - (char*)b); }
void *map_id_copy(void *p) { return p; }
void map_id_free(void *p) {}

#ifdef UTIL_ITER
// Forward declarations for private functions
bool __map_iter_next_pair(iter_t *iter, void *key, void *value);
bool __map_iter_next(iter_t *iter, void *value);

struct map_iter_t {
    iter_t iter;
    map_t *map;
    struct map_node_t *node;
    i32 n;
} map_iter_t;

bool __map_iter_next_pair(iter_t *iter, void *key, void *value) {
   struct map_iter_t *h_iter = (struct map_iter_t*)iter;
    while(!h_iter->node && h_iter->n <= h_iter->map->capacity)
        h_iter->node = h_iter->map->nodes[h_iter->n++];
    if(h_iter->n > h_iter->map->capacity) return 0;
    *(void**)key = h_iter->node->key;
    *(void**)value = h_iter->node->value;
    h_iter->node = h_iter->node->next;
    return 1;
}

iter_t *map_iter_pairs(map_t *map) {
    struct map_iter_t *ret = calloc(1, sizeof(map_iter_t));
    ret->iter.next_pair = __map_iter_next_pair;
    ret->map = map;
    return (iter_t*)ret;
}

bool __map_iter_next(iter_t *iter, void *value) {
    void *key;
    return __map_iter_next_pair(iter, &key, value);
}

iter_t *map_iter(map_t *map) {
    struct map_iter_t *ret = calloc(1, sizeof(map_iter_t));
    ret->iter.next = __map_iter_next;
    ret->map = map;
    return (iter_t*)ret;
}
#endif
#endif

#ifdef UTIL_JSON
typedef struct json_t {
    i32 type;
    union {
        i32 value_int;
        f32 value_float;
        char *value_str;
        struct json_arr_t {
            struct json_arr_t *next;
            struct json_arr_t *prev;
            struct json_t *value;
        } *value_arr;
        struct json_obj_t {
            struct json_obj_t *next;
            struct json_obj_t *prev;
            struct json_t *value;
            char *tag;
        } *value_obj;
    };
} json_t;

// Forward declarations for private functions
void __json_skip_ws(char *buf, int *i);
void __json_free_internal(json_t *j);
char *__json_parse_str(char *buf, int *i);
json_t *__json_parse_any(char *buf, int *i);
json_t *__json_parse_complex(char *buf, int *i, bool arr);

void __json_skip_ws(char *buf, int *i) {
    while(1) {
        switch (buf[*i]) {
            case ' ':
            case '\t':
            case '\r':
            case '\n': break;
            default: return;
        }
        (*i)++;
    }
}

void __json_free_internal(json_t *j) {
    switch (j->type) {
        case JSON_OBJECT: {
            struct json_obj_t *ptr = j->value_obj, *start = ptr;
            if(!ptr) break;
            do {
                struct json_obj_t *next = ptr->next;
                json_free(ptr->value);
                free(ptr->tag);
                free(ptr);
                ptr = next;
            } while(ptr != start);
        } break;
        case JSON_ARRAY: {
            struct json_arr_t *ptr = j->value_arr, *start = ptr;
            if(!ptr) break;
            do {
                struct json_arr_t *next = ptr->next;
                json_free(ptr->value);
                free(ptr);
                ptr = next;
            } while(ptr != start);
        } break;
        case JSON_BOOL:
        case JSON_NULL:
        case JSON_FLOAT:
        case JSON_INT: break;
        case JSON_STRING: free(j->value_str); break;
        default: return;
    }
}

char *__json_parse_str(char *buf, int *i) {
    int size = 0;
    (*i)++;
    static char tmp[256];
    while(buf[*i] && buf[*i] != '\"') {
        if(buf[*i] == '\\') {
            (*i)++;
            switch(buf[*i]) {
                case '"': tmp[size++] = '"'; break;
                case '\\': tmp[size++] = '\\'; break;
                case 'b': tmp[size++] = '\b'; break;
                case 'f': tmp[size++] = '\f'; break;
                case 'n': tmp[size++] = '\n'; break;
                case 'r': tmp[size++] = '\r'; break;
                case 't': tmp[size++] = '\t'; break;
                default: (*i)--; break;
            }
        }
        else tmp[size++] = buf[*i];
        (*i)++;
    }
    char *ret = malloc(size + 1);
    memcpy(ret, tmp, size);
    ret[size] = 0;
    (*i)++;
    return ret;
}

json_t *__json_parse_complex(char *buf, int *i, bool arr) {
    (*i)++;
    json_t *next, *ret = calloc(1, sizeof(json_t));
    ret->type = arr;
    char end = "}]"[arr];
    ret->value_obj = 0;
    __json_skip_ws(buf, i);
    if(buf[*i] == end) return ret;
    while(1) {
        char *tag = 0;
        if(!arr) {
            if(buf[*i] != '\"') {
                free(ret);
                return 0;
            }
            tag = __json_parse_str(buf, i);
            __json_skip_ws(buf, i);
            if(buf[(*i)++] != ':') {
                free(tag);
                json_free(ret);
                return 0;
            }
        }
        next = __json_parse_any(buf, i);
        if(!next) {
            if(tag) free(tag);
            json_free(ret);
            return 0;
        }
        if(arr) json_arr_add(ret, -1, next);
        else json_obj_add(ret, tag, next);
        __json_skip_ws(buf, i);
        if(buf[*i] == end) break;
        if(buf[*i] != ',') {
            json_free(ret);
            return 0;
        }
        (*i)++;
        __json_skip_ws(buf, i);
    }
    (*i)++;
    return ret;
}

json_t *__json_parse_any(char *buf, int *i) {
    for(; buf[*i]; (*i)++) {
        switch (buf[*i]) {
            case '{': return __json_parse_complex(buf, i, 0);
            case '[': return __json_parse_complex(buf, i, 1);
            case '\"': {
                json_t *ret = calloc(1, sizeof(json_t));
                ret->type = JSON_STRING;
                ret->value_str = __json_parse_str(buf, i);
                return ret;
            } break;
            case 't': {
                json_t *ret = calloc(1, sizeof(json_t));
                if(strncmp(&buf[*i + 1], "rue", 3)) {
                    free(ret);
                    return 0;
                }
                *i += 3;
                ret->type = JSON_BOOL;
                ret->value_int = 1;
                return ret;
            }
            case 'f': {
                json_t *ret = calloc(1, sizeof(json_t));
                if(strncmp(&buf[*i + 1], "alse", 4)) {
                    free(ret);
                    return 0;
                }
                *i += 4;
                ret->type = JSON_BOOL;
                ret->value_int = 0;
                return ret;
            }
            case '-':
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9': {
                json_t *ret = calloc(1, sizeof(json_t));
                int index;
                if(sscanf(&buf[*i], "%d%n", &ret->value_int, &index) == 1) {
                    index += *i;
                    if(buf[index] == '.' || buf[index] == 'e' || buf[index] == 'E') {
                        if(sscanf(&buf[*i], "%f%n", &ret->value_float, &index) == 1) {
                            ret->type = JSON_FLOAT;
                            *i += index;
                            return ret;
                        }
                    } else {
                        ret->type = JSON_INT;
                        *i = index;
                        return ret;
                    }
                }
                free(ret);
                return 0;
            } break;
            case ' ':
            case '\t':
            case '\r':
            case '\n': break;
            default: return 0;
        }
    }
    return 0;
}

json_t *json_parse(char *buffer) {
    int index = 0;
    return __json_parse_any(buffer, &index);
}

i32 json_type(json_t *j) { return j->type; }

void json_set(json_t *j, json_t *new) {
    __json_free_internal(j);
    *j = *new;
    free(new);
}

void json_free(json_t *j) {
    __json_free_internal(j);
    free(j);
}

json_t *json_new_float(f32 f) {
    json_t *ret = calloc(1, sizeof(json_t));
    ret->type = JSON_FLOAT;
    ret->value_float = f;
    return ret;
}

bool json_get_float(json_t *j, float *f) {
    if(!j || j->type != JSON_FLOAT) return 0;
    *f = j->value_float;
    return 1;
}

json_t *json_new_int(i32 i) {
    json_t *ret = calloc(1, sizeof(json_t));
    ret->type = JSON_INT;
    ret->value_int = i;
    return ret;
}

bool json_get_int(json_t *j, i32 *i) {
    if(!j) return 0;
    switch(j->type) {
        case JSON_INT:
        case JSON_BOOL: *i = j->value_int; return 1;
        case JSON_FLOAT: *i = (int)(j->value_float + 0.5f); return 1;
        default: return 0;
    }
}

json_t *json_new_str(char *c) {
    json_t *ret = calloc(1, sizeof(json_t));
    ret->type = JSON_STRING;
    ret->value_str = strdup(c);
    return ret;
}

bool json_get_str(json_t *j, char **ret) {
    if(!j || j->type != JSON_STRING) return 0;
    *ret = j->value_str;
    return 1;
}

json_t *json_new_obj() {
    json_t *ret = calloc(1, sizeof(json_t));
    ret->type = JSON_OBJECT;
    return ret;
}

json_t *json_obj_get(json_t *j, char *name) {
    if(!j || j->type != JSON_OBJECT) return 0;
    struct json_obj_t *ptr = j->value_obj, *start = ptr;
    if(!ptr) return 0;
    do {
        if(!strcmp(name, ptr->tag)) return ptr->value;
        ptr = ptr->next;
    } while(ptr != start);
    return 0;
}
void json_obj_add(json_t *j, char *name, json_t *new) {
    if(!j || j->type != JSON_OBJECT) return;
    struct json_obj_t *ptr = calloc(1, sizeof(struct json_obj_t));
    ptr->value = new;
    ptr->tag = strdup(name);
    if(!j->value_obj) {
        j->value_obj = ptr;
        j->value_obj->next = ptr;
        j->value_obj->prev = ptr;
        return;
    }
    struct json_obj_t *next, *prev;
    next = j->value_obj; // next is current head
    prev = j->value_obj->prev; // prev is the last element
    prev->next = ptr;
    next->prev = ptr;
    ptr->next = next;
    ptr->prev = prev;
}

json_t *json_obj_del(json_t *j, char *name) {
    if(!j || j->type != JSON_OBJECT) return 0;
    struct json_obj_t *ptr = j->value_obj, *start = ptr;
    while(strcmp(name, ptr->tag)) {
        ptr = ptr->next;
        if(ptr == start) return 0;
    }
    if(ptr == ptr->next) { // ptr is the only element
        j->value_obj = 0;
    } else {
        ptr->prev->next = ptr->next;
        ptr->next->prev = ptr->prev;
        if (ptr == j->value_obj) j->value_obj = ptr->next;
    }
    json_t *ret = ptr->value;
    free(ptr->tag);
    free(ptr);
    return ret;
}

json_t *json_new_arr() {
    json_t *ret = calloc(1, sizeof(json_t));
    ret->type = JSON_ARRAY;
    return ret;
}

json_t *json_arr_get(json_t *j, i32 i) {
    if(!j || j->type != JSON_ARRAY) return 0;
    struct json_arr_t *ptr = j->value_arr;
    for(int n = 0; ptr && n < i; n++) ptr = ptr->next;
    return ptr ? ptr->value : 0;
}

void json_arr_add(json_t *j, i32 i, json_t *new) {
    if(!j || j->type != JSON_ARRAY) return;
    struct json_arr_t *ins = calloc(1, sizeof(struct json_arr_t));
    ins->value = new;
    if(!j->value_arr) {
        j->value_arr = ins;
        ins->next = ins;
        ins->prev = ins;
        return;
    }
    struct json_arr_t *prev = j->value_arr->prev;
    struct json_arr_t *next = j->value_arr;
    for(i32 n = 0; n > i; n--) {
        next = prev;
        prev = prev->prev;
    }
    for(i32 n = 0; n < i; n++) {
        prev = next;
        next = next->next;
    }
    if(!i) j->value_arr = ins;
    prev->next = ins;
    next->prev = ins;
    ins->next = next;
    ins->prev = prev;
}

json_t *json_arr_del(json_t *j, i32 i) {
    if(!j || j->type != JSON_ARRAY) return 0;
    struct json_arr_t *ptr = j->value_arr;
    for(int n = 0; ptr && n < i; n++) ptr = ptr->next;
    json_t *ret = ptr->value;
    if(ptr->prev) ptr->prev->next = ptr->next;
    if(ptr->next) ptr->next->prev = ptr->prev;
    if(!i) j->value_arr = ptr->next;
    free(ptr);
    return ret;
}

void __json_fprint_indent(FILE *f, char *indent, int n) {
    for(int i = 0; i < n; i++) fprintf(f, "%s", indent);
}

void __json_fprint_ex(json_t *j, FILE *f, char *indent, int n) {
    switch (j->type) {
        case JSON_OBJECT: {
            fprintf(f, indent ? "{\n" : "{");
            struct json_obj_t *ptr = j->value_obj, *start = ptr;
            while(1) {
                __json_fprint_indent(f, indent, n + 1);
                fprintf(f, "\"%s\": ", ptr->tag);
                __json_fprint_ex(ptr->value, f, indent, n + 1);
                ptr = ptr->next;
                if(ptr != start) fprintf(f, indent ? ",\n" : ",");
                else break;
            }
            if(indent) fprintf(f, "\n");
            __json_fprint_indent(f, indent, n);
            fprintf(f, "}");
        } break;
        case JSON_ARRAY: {
            fprintf(f, indent ? "[\n" : "]");
            struct json_arr_t *ptr = j->value_arr, *start = ptr;
            while(1) {
                __json_fprint_indent(f, indent, n + 1);
                __json_fprint_ex(ptr->value, f, indent, n + 1);
                ptr = ptr->next;
                if(ptr != start) fprintf(f, indent ? ",\n" : ",");
                else break;
            }
            if(indent) fprintf(f, "\n");
            __json_fprint_indent(f, indent, n);
            fprintf(f, "]");
        } break;
        case JSON_BOOL: fprintf(f, "%s", j->value_int ? "true" : "false"); break;
        case JSON_NULL: fprintf(f, "null"); break;
        case JSON_FLOAT: fprintf(f, "%f", j->value_float); break;
        case JSON_INT: fprintf(f, "%d", j->value_int); break;
        case JSON_STRING: {
            fprintf(f, "\"");
            for(int i = 0; j->value_str[i]; i++) {
                switch (j->value_str[i]) {
                    case '"': fprintf(f, "\\\""); break;
                    case '\\': fprintf(f, "\\\\"); break;
                    case '\b': fprintf(f, "\\b"); break;
                    case '\f': fprintf(f, "\\f"); break;
                    case '\n': fprintf(f, "\\n"); break;
                    case '\r': fprintf(f, "\\r"); break;
                    case '\t': fprintf(f, "\\t"); break;
                    default: fprintf(f, "%c", j->value_str[i]); break;
                }
            }
            fprintf(f, "\"");
        } break;
        default: return;
    }
}

void json_fprint(json_t *j, FILE *f, char *indent) {
    __json_fprint_ex(j, f, indent, 0);
    fprintf(f, "\n");
}

#ifdef UTIL_ITER
// Forward declarations for private functions
bool __json_iter_obj_next(iter_t *iter, void *name);
bool __json_iter_arr_next(iter_t *iter, void *json);

struct json_iter_t {
    iter_t iter;
    json_t *j;
    union {
        struct json_arr_t *arr;
        struct json_obj_t *obj;
    };
};

bool __json_iter_obj_next(iter_t *iter, void *entry) {
    struct json_iter_t *j_iter = (struct json_iter_t*)iter;
    if(j_iter->j->value_obj == j_iter->obj) return 0;
    if(!j_iter->obj) j_iter->obj = j_iter->j->value_obj;
    ((json_entry_t*)entry)->name = j_iter->obj->tag;
    ((json_entry_t*)entry)->value = j_iter->obj->value;
    j_iter->obj = j_iter->obj->next;
    return 1;
}

iter_t *json_iter_obj(json_t *j) {
    if(!j || j->type != JSON_OBJECT) return 0;
    struct json_iter_t *j_iter = calloc(1, sizeof(struct json_iter_t));
    j_iter->j = j;
    j_iter->iter.next = __json_iter_obj_next;
    return (iter_t*)j_iter;
}

bool __json_iter_arr_next(iter_t *iter, void *json) {
    struct json_iter_t *j_iter = (struct json_iter_t*)iter;
    if(j_iter->j->value_arr == j_iter->arr) return 0;
    if(!j_iter->arr) j_iter->arr = j_iter->j->value_arr;
    *(json_t**)json = j_iter->arr->value;
    j_iter->arr = j_iter->arr->next;
    return 1;
}

// iterates json_t*
iter_t *json_iter_arr(json_t *j) {
    if(!j || j->type != JSON_ARRAY) return 0;
    struct json_iter_t *j_iter = calloc(1, sizeof(struct json_iter_t));
    j_iter->j = j;
    j_iter->iter.next = __json_iter_arr_next;
    return (iter_t*)j_iter;
}
#endif
#endif
