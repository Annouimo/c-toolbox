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
