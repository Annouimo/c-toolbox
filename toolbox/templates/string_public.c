/* SPDX-License-Identifier: MIT-0 */
typedef struct {bool ok;String value;} StringResult;
static StringResult string_list_try_pop(StringList *list){StringResult r={false,{0}};r.ok=string_list_pop_into(list,&r.value);return r;}
static String string_list_pop(StringList *list){StringResult r=string_list_try_pop(list);if(!r.ok)tb_fail("pop of empty string list");return r.value;}
typedef struct {size_t begin,end;} StringRun;
static StringRun string_longest_char_run(const String *s,char target){StringRun best={0,0};size_t start=0;for(size_t i=0;i<s->len;++i){if(s->data[i]!=target)start=i+1;else if(i+1-start>best.end-best.begin)best=(StringRun){start,i+1};}return best;}
static void print_strings(const StringList *words,const char *separator,const char *end){for(size_t i=0;i<words->len;++i){if(i)print_text(separator);print_string(&words->data[i]);}print_text(end);}
