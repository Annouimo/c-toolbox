/* SPDX-License-Identifier: MIT-0 */
typedef struct {bool ok;long long value;} MapResult;
static MapResult map_try_get(const Map *m,const char *key){MapResult r={false,0};r.ok=map_get_into(m,key,&r.value);return r;}
static long long map_get(const Map *m,const char *key){MapResult r=map_try_get(m,key);if(!r.ok)tb_fail("missing map key");return r.value;}
static MapResult map_try_get_s(const Map *m,const String *key){return map_try_get(m,string_cstr(key));}
static long long map_get_s(const Map *m,const String *key){return map_get(m,string_cstr(key));}

/* Iteration cursor is updated in place; entry fields are returned. */
typedef struct {bool ok;const char *key;long long value;} MapEntryView;
static MapEntryView map_next(const Map *m,size_t *cursor){MapEntryView r={false,NULL,0};r.ok=map_next_into(m,cursor,&r.key,&r.value);return r;}
