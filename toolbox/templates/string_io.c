/* SPDX-License-Identifier: MIT-0 */
typedef struct { IOStatus status; String value; } StringReadResult;
typedef struct { bool ok; int value; } StringIntResult;
typedef struct { bool ok; long long value; } StringLongLongResult;
typedef struct { bool ok; double value; } StringDoubleResult;
static StringIntResult string_parse_int(const String *s) {
    StringIntResult r={false,0};if(!s->len)return r;char *end;errno=0;long long x=strtoll(string_cstr(s),&end,10);
    if(errno==ERANGE || end==string_cstr(s) || *end || x<INT_MIN || x>INT_MAX)return r;
    r.ok=true;r.value=(int)x;return r;
}
static StringLongLongResult string_parse_ll(const String *s) {
    StringLongLongResult r={false,0};if(!s->len)return r;char *end;errno=0;long long x=strtoll(string_cstr(s),&end,10);
    if(errno==ERANGE || end==string_cstr(s) || *end)return r;
    r.ok=true;r.value=x;return r;
}
static StringDoubleResult string_parse_double(const String *s) {
    StringDoubleResult r={false,0};if(!s->len)return r;char *end;errno=0;double x=strtod(string_cstr(s),&end);
    if(errno==ERANGE || end==string_cstr(s) || *end || !isfinite(x))return r;
    r.ok=true;r.value=x;return r;
}
static StringReadResult io_try_read_word(IOReader *r) {
    StringReadResult result={IO_OK,{0}};char *token=tb_io_token(r,&result.status);
    if(result.status==IO_OK)result.value=string_from(token);
    free(token);return result;
}
/* Reads one physical line/remainder, preserving spaces and empty lines. */
static StringReadResult io_try_read_line_raw(IOReader *r) {
    StringReadResult result={IO_OK,{0}};int c;bool any=false;
    while((c=fgetc(r->stream))!=EOF) {
        any=true;if(c=='\n')break;
        if(c=='\r'){int next=fgetc(r->stream);if(next!=EOF && next!='\n' && ungetc(next,r->stream)==EOF)result.status=IO_STREAM_ERROR;break;}
        if(!c)result.status=IO_FORMAT;else string_push(&result.value,(char)c);
    }
    r->line_pending=false;
    if(ferror(r->stream))result.status=IO_STREAM_ERROR;
    else if(!any)result.status=IO_EOF;
    if(result.status!=IO_OK)string_free(&result.value);
    return result;
}
static StringReadResult io_try_read_line(IOReader *r) {
    for(;;){StringReadResult result=io_try_read_line_raw(r);if(result.status!=IO_OK)return result;string_trim(&result.value);if(result.value.len)return result;string_free(&result.value);}
}
static StringReadResult try_read_word(void){return io_try_read_word(tb_input());}
static StringReadResult try_read_line(void){return io_try_read_line(tb_input());}
static StringReadResult try_read_line_raw(void){return io_try_read_line_raw(tb_input());}
static String read_word(void){StringReadResult r=try_read_word();io_require(r.status);return r.value;}
static String read_line(void){StringReadResult r=try_read_line();io_require(r.status);return r.value;}
static String read_line_raw(void){StringReadResult r=try_read_line_raw();io_require(r.status);return r.value;}
static String read_rest(void){return read_line_raw();}
static void print_string(const String *s,const char *end){print_text(string_cstr(s),end);}

/* After a token, reject extra nonspace data before crossing into next line. */
static IOStatus io_finish_line(IOReader *r) {
    if(!r->line_pending)return IO_OK;
    int c;
    while((c=fgetc(r->stream))!=EOF){
        if(c=='\n')break;
        if(c=='\r'){int n=fgetc(r->stream);if(n!=EOF && n!='\n' && ungetc(n,r->stream)==EOF)return IO_STREAM_ERROR;break;}
        if(!isspace((unsigned char)c)){if(ungetc(c,r->stream)==EOF)return IO_STREAM_ERROR;return IO_EXTRA_DATA;}
    }
    r->line_pending=false;return ferror(r->stream)?IO_STREAM_ERROR:IO_OK;
}
static StringReadResult try_read_next_line_raw(void){IOStatus s=io_finish_line(tb_input());if(s!=IO_OK){StringReadResult r={s,{0}};return r;}return try_read_line_raw();}
static StringReadResult try_read_next_line(void){IOStatus s=io_finish_line(tb_input());if(s!=IO_OK){StringReadResult r={s,{0}};return r;}return try_read_line();}
static String read_next_line_raw(void){StringReadResult r=try_read_next_line_raw();io_require(r.status);return r.value;}
static String read_next_line(void){StringReadResult r=try_read_next_line();io_require(r.status);return r.value;}
