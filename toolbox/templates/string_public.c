/* SPDX-License-Identifier: MIT-0 */
typedef struct {size_t begin,end;} StringRun;
static StringRun string_longest_char_run(const String *s,char target){StringRun best={0,0};size_t start=0;for(size_t i=0;i<s->len;++i){if(s->data[i]!=target)start=i+1;else if(i+1-start>best.end-best.begin)best=(StringRun){start,i+1};}return best;}
