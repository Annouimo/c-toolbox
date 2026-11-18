/* SPDX-License-Identifier: MIT-0 */
#ifndef CTB_IO_H
#define CTB_IO_H
#include "detail/core.h"
#include <errno.h>
#include <math.h>
#include <stdarg.h>
typedef enum { IO_OK, IO_EOF, IO_FORMAT, IO_RANGE, IO_STREAM_ERROR, IO_EXTRA_DATA } IOStatus;
typedef struct { FILE *stream; bool line_pending; } IOReader;
static IOReader io_reader(FILE *stream) { IOReader r = {stream, false}; return r; }
static IOReader *tb_input(void) { static IOReader r; if (!r.stream) r.stream=stdin; return &r; }
static void io_require(IOStatus s) { if(s!=IO_OK) tb_fail(s==IO_EOF?"unexpected EOF":s==IO_RANGE?"input out of range":s==IO_FORMAT?"invalid input":"stream error"); }
/* Internal owned token; consumes a complete token and retains its delimiter. */
static char *tb_io_token(IOReader *r, IOStatus *status) {
    int c; do { c=fgetc(r->stream); } while(c!=EOF && isspace((unsigned char)c));
    *status=IO_OK;
    if(c==EOF) { *status=ferror(r->stream)?IO_STREAM_ERROR:IO_EOF; return NULL; }
    size_t n=0, cap=32; char *s=tb_resize(NULL,cap,1);
    do { if(!c) *status=IO_FORMAT; if(n==cap-1) {cap=tb_capacity(cap,tb_add_size(n,2));s=tb_resize(s,cap,1);} s[n++]=(char)c; c=fgetc(r->stream); } while(c!=EOF && !isspace((unsigned char)c));
    s[n]=0; r->line_pending=true;
    if(ferror(r->stream) || (c!=EOF && ungetc(c,r->stream)==EOF)) *status=IO_STREAM_ERROR;
    return s;
}
typedef struct { IOStatus status; long long value; } LongLongReadResult;
static LongLongReadResult io_try_read_ll(IOReader *r) {
    LongLongReadResult result={IO_OK,0}; char *s=tb_io_token(r,&result.status);
    if(result.status==IO_OK) { char *end; errno=0; long long value=strtoll(s,&end,10);
        if(end==s || *end) result.status=IO_FORMAT;
        else if(errno==ERANGE) result.status=IO_RANGE;
        else {  }
        if(result.status==IO_OK) result.value=(long long)value;
    } free(s); return result;
}
static LongLongReadResult try_read_ll(void) { return io_try_read_ll(tb_input()); }
static long long read_ll(void) { LongLongReadResult r=try_read_ll(); io_require(r.status); return r.value; }
typedef struct { IOStatus status; unsigned long long value; } ULongLongReadResult;
static ULongLongReadResult io_try_read_ull(IOReader *r) {
    ULongLongReadResult result={IO_OK,0}; char *s=tb_io_token(r,&result.status);
    if(result.status==IO_OK) { char *end; errno=0; unsigned long long value=strtoull(s,&end,10);
        if(end==s || *end) result.status=IO_FORMAT;
        else if(errno==ERANGE) result.status=IO_RANGE;
        else { if(s[0]=='-') result.status=IO_RANGE; }
        if(result.status==IO_OK) result.value=(unsigned long long)value;
    } free(s); return result;
}
static ULongLongReadResult try_read_ull(void) { return io_try_read_ull(tb_input()); }
static unsigned long long read_ull(void) { ULongLongReadResult r=try_read_ull(); io_require(r.status); return r.value; }
typedef struct { IOStatus status; int value; } IntReadResult;
static IntReadResult io_try_read_int(IOReader *r) {
    IntReadResult result={IO_OK,0}; char *s=tb_io_token(r,&result.status);
    if(result.status==IO_OK) { char *end; errno=0; long long value=strtoll(s,&end,10);
        if(end==s || *end) result.status=IO_FORMAT;
        else if(errno==ERANGE) result.status=IO_RANGE;
        else { if(value<INT_MIN || value>INT_MAX) result.status=IO_RANGE; }
        if(result.status==IO_OK) result.value=(int)value;
    } free(s); return result;
}
static IntReadResult try_read_int(void) { return io_try_read_int(tb_input()); }
static int read_int(void) { IntReadResult r=try_read_int(); io_require(r.status); return r.value; }
typedef struct { IOStatus status; unsigned int value; } UIntReadResult;
static UIntReadResult io_try_read_uint(IOReader *r) {
    UIntReadResult result={IO_OK,0}; char *s=tb_io_token(r,&result.status);
    if(result.status==IO_OK) { char *end; errno=0; unsigned long long value=strtoull(s,&end,10);
        if(end==s || *end) result.status=IO_FORMAT;
        else if(errno==ERANGE) result.status=IO_RANGE;
        else { if(s[0]=='-' || value>UINT_MAX) result.status=IO_RANGE; }
        if(result.status==IO_OK) result.value=(unsigned int)value;
    } free(s); return result;
}
static UIntReadResult try_read_uint(void) { return io_try_read_uint(tb_input()); }
static unsigned int read_uint(void) { UIntReadResult r=try_read_uint(); io_require(r.status); return r.value; }
typedef struct { IOStatus status; double value; } DoubleReadResult;
static DoubleReadResult io_try_read_double(IOReader *r) {
    DoubleReadResult result={IO_OK,0}; char *s=tb_io_token(r,&result.status);
    if(result.status==IO_OK) { char *end; errno=0; double value=strtod(s,&end);
        if(end==s || *end) result.status=IO_FORMAT;
        else if(errno==ERANGE) result.status=IO_RANGE;
        else { if(!isfinite(value)) result.status=IO_RANGE; }
        if(result.status==IO_OK) result.value=(double)value;
    } free(s); return result;
}
static DoubleReadResult try_read_double(void) { return io_try_read_double(tb_input()); }
static double read_double(void) { DoubleReadResult r=try_read_double(); io_require(r.status); return r.value; }
static bool read_bool(void) {
    int value = read_int();
    if (value != 0 && value != 1) io_require(IO_FORMAT);
    return value == 1;
}
typedef struct { IOStatus status; char value; } CharReadResult;
static CharReadResult io_try_read_char(IOReader *r, bool raw) {
    int c; do { c=fgetc(r->stream); } while(!raw && c!=EOF && isspace((unsigned char)c));
    if(c!=EOF) r->line_pending = c!='\n';
    CharReadResult result={c==EOF?(ferror(r->stream)?IO_STREAM_ERROR:IO_EOF):IO_OK,c==EOF?0:(char)c};return result;
}
static CharReadResult try_read_char(void) { return io_try_read_char(tb_input(),false); }
static CharReadResult try_read_char_raw(void) { return io_try_read_char(tb_input(),true); }
static char read_char(void) { CharReadResult r=try_read_char();io_require(r.status);return r.value; }
static char read_char_raw(void) { CharReadResult r=try_read_char_raw();io_require(r.status);return r.value; }
static void io_reader_skip_line(IOReader *reader) {
    FILE *f=reader->stream;int c;
    while((c=fgetc(f))!=EOF && c!='\n' && c!='\r') {}
    if(c=='\r') {c=fgetc(f);if(c!=EOF && c!='\n' && ungetc(c,f)==EOF) io_require(IO_STREAM_ERROR);}
    reader->line_pending=false;
    if(ferror(f)) io_require(IO_STREAM_ERROR);
}
static void io_skip_line(void) { io_reader_skip_line(tb_input()); }
static void io_reader_skip_whitespace(IOReader *reader) {
    FILE *f=reader->stream; int c;bool crossed_line=false;
    do {c=fgetc(f);if(c=='\n'||c=='\r')crossed_line=true;} while(c!=EOF && isspace((unsigned char)c));
    if(ferror(f) || (c!=EOF && ungetc(c,f)==EOF)) io_require(IO_STREAM_ERROR);
    if(crossed_line)reader->line_pending=false;
}
static void io_skip_whitespace(void) { io_reader_skip_whitespace(tb_input()); }
enum { PRINT_AUTO = -1 };
static void print_format(const char *format,const char *end,...) { va_list a;va_start(a,end);int n=vprintf(format,a);va_end(a);if(n<0)io_require(IO_STREAM_ERROR);if(fputs(end,stdout)==EOF)io_require(IO_STREAM_ERROR); }
static void print_int(int v,const char *end) {print_format("%d",end,v);}
static void print_ll(long long v,const char *end) {print_format("%lld",end,v);}
static void print_uint(unsigned int v,const char *end) {print_format("%u",end,v);}
static void print_ull(unsigned long long v,const char *end) {print_format("%llu",end,v);}
static void print_fixed(double v,int digits,const char *end) {
    if(digits<0)tb_fail("negative precision");
    int n=snprintf(NULL,0,"%.*f",digits,v);if(n<0)tb_fail("decimal formatting failed");
    char *text=tb_resize(NULL,tb_add_size((size_t)n,1),1);snprintf(text,(size_t)n+1,"%.*f",digits,v);
    bool zero=text[0]=='-';for(size_t i=1;zero && text[i];++i)if(text[i]!='0' && text[i]!='.')zero=false;
    int status=fputs(text+(zero?1:0),stdout);free(text);if(status==EOF)io_require(IO_STREAM_ERROR);if(fputs(end,stdout)==EOF)io_require(IO_STREAM_ERROR);
}
static void print_double(double v,int digits,const char *end) {
    if(digits==PRINT_AUTO) {
        char text[64];int n=snprintf(text,sizeof text,"%.17g",v);if(n<0 || (size_t)n>=sizeof text)tb_fail("decimal formatting failed");
        bool zero=text[0]=='-';for(size_t i=1;zero && text[i];++i)if(text[i]!='0' && text[i]!='.')zero=false;
        if(fputs(text+(zero?1:0),stdout)==EOF)io_require(IO_STREAM_ERROR);
        if(fputs(end,stdout)==EOF)io_require(IO_STREAM_ERROR);
    } else print_fixed(v,digits,end);
}
static void print_text(const char *s,const char *end) {if(fputs(s,stdout)==EOF || fputs(end,stdout)==EOF)io_require(IO_STREAM_ERROR);}
static void print_char(char c,const char *end) {if(putchar((unsigned char)c)==EOF || fputs(end,stdout)==EOF)io_require(IO_STREAM_ERROR);}
static void io_flush(void) {if(fflush(stdout)==EOF)io_require(IO_STREAM_ERROR);}
#endif
