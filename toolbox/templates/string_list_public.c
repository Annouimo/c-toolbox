/* SPDX-License-Identifier: MIT-0 */
/* Reapply each deletion pattern until reaching a fixed point across the set. */
static size_t string_remove_patterns(String *text,const StringList *patterns) {
    size_t total=0,pass;
    do { pass=0;for(size_t i=0;i<patterns->len;++i)pass+=string_remove_repeated(text,string_cstr(&patterns->data[i]));total+=pass; } while(pass);
    return total;
}
typedef struct {bool ok;String value;} StringResult;
static StringResult string_list_try_pop(StringList *list){StringResult r={false,{0}};r.ok=string_list_pop_into(list,&r.value);return r;}
static String string_list_pop(StringList *list){StringResult r=string_list_try_pop(list);if(!r.ok)tb_fail("pop of empty string list");return r.value;}
static void print_strings(const StringList *words,const char *separator,const char *end){for(size_t i=0;i<words->len;++i){if(i)print_text(separator,"");print_string(&words->data[i],"");}print_text("",end);}
