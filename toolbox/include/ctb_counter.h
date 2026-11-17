#ifndef CTB_COUNTER_H
#define CTB_COUNTER_H
#include "ctb_map.h"
/* SPDX-License-Identifier: MIT-0 */
typedef struct { Map items; } Counter;
static Counter counter_new(void) { return (Counter){0}; }
/* Missing keys read as zero. Zero/negative counts are retained, like Python Counter. */
static long long counter_get(const Counter *c, const char *key) {
    long long value = 0;
    (void)map_get_into(&c->items, key, &value);
    return value;
}
/* false on overflow; leaves the counter unchanged. */
static bool counter_add(Counter *c, const char *key, long long delta) {
    long long value = counter_get(c, key);
    if ((delta > 0 && value > LLONG_MAX - delta) ||
        (delta < 0 && value < LLONG_MIN - delta)) return false;
    (void)map_set(&c->items, key, value + delta);
    return true;
}
static bool counter_remove(Counter *c, const char *key) { return map_remove(&c->items, key); }
static bool counter_next_into(const Counter *c, size_t *cursor, const char **key, long long *value) {
    return map_next_into(&c->items, cursor, key, value);
}
static Counter counter_clone(const Counter *c) { Counter copy = {map_clone(&c->items)}; return copy; }
static void counter_free(Counter *c) { map_free(&c->items); }
static void counter_clear(Counter *c) { map_clear(&c->items); }
static size_t counter_size(const Counter *c) { return c->items.len; }
/* Highest count; ties choose lexicographically smallest key. Borrowed key. */
static bool counter_most_common_into(const Counter *c, const char **out_key, long long *out_count) {
    size_t cursor = 0;
    const char *key, *best_key = NULL;
    long long value, best = 0;
    while (counter_next_into(c, &cursor, &key, &value)) {
        if (!best_key || value > best || (value == best && strcmp(key, best_key) < 0)) {
            best_key = key; best = value;
        }
    }
    if (!best_key) return false;
    if (out_key) *out_key = best_key;
    if (out_count) *out_count = best;
    return true;
}
static bool counter_add_s(Counter *c, const String *key, long long delta) {
    return counter_add(c, string_cstr(key), delta);
}
static long long counter_get_s(const Counter *c, const String *key) {
    return counter_get(c, string_cstr(key));
}
static bool counter_remove_s(Counter *c, const String *key) {
    return counter_remove(c, string_cstr(key));
}
static StringList counter_keys(const Counter *c) { return map_keys(&c->items); }
static Counter counter_from_strings(const StringList *words) {
    Counter result = counter_new();
    for (size_t i = 0; i < words->len; ++i)
        if (!counter_add_s(&result, string_list_get(words, i), 1)) tb_fail("counter overflow");
    return result;
}
static bool counter_most_common_string_into(const Counter *c, String *out_key, long long *out_count) {
    const char *key;
    long long count;
    if (!counter_most_common_into(c, &key, &count)) return false;
    if (out_key) string_assign(out_key, key);
    if (out_count) *out_count = count;
    return true;
}


/* SPDX-License-Identifier: MIT-0 */
/* Borrowed key, invalidated by modification/free of the counter. */
typedef struct {bool ok;const char *key;long long count;} CounterEntry;
static CounterEntry counter_most_common(const Counter *c){CounterEntry r={false,NULL,0};r.ok=counter_most_common_into(c,&r.key,&r.count);return r;}
/* Owned String, free result.key even when the result is empty. */
typedef struct {bool ok;String key;long long count;} CounterStringEntry;
static CounterStringEntry counter_most_common_string(const Counter *c){CounterStringEntry r={false,{0},0};r.ok=counter_most_common_string_into(c,&r.key,&r.count);return r;}
/* Integer-key adapter. Keys are canonical decimal strings internally. */
typedef struct {Counter items;} IntCounter;
static IntCounter int_counter_new(void){return (IntCounter){0};}
static void tb_integer_key(char *buffer,size_t size,long long key){snprintf(buffer,size,"%lld",key);}
static bool int_counter_add(IntCounter *c,long long key,long long delta){char s[sizeof(long long)*CHAR_BIT+2];tb_integer_key(s,sizeof s,key);return counter_add(&c->items,s,delta);}
static long long int_counter_get(const IntCounter *c,long long key){char s[sizeof(long long)*CHAR_BIT+2];tb_integer_key(s,sizeof s,key);return counter_get(&c->items,s);}
static void int_counter_free(IntCounter *c){counter_free(&c->items);}

static CounterEntry counter_next(const Counter *c,size_t *cursor){CounterEntry r={false,NULL,0};r.ok=counter_next_into(c,cursor,&r.key,&r.count);return r;}

#endif
