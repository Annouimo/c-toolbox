#include "ctb/io.h"
#include <assert.h>
int main(void) {
 FILE *f=tmpfile(); assert(f); fputs("12 12x -1 4294967296 2.5\r\n Z",f);rewind(f);IOReader r=io_reader(f);
 IntReadResult a=io_try_read_int(&r);assert(a.status==IO_OK && a.value==12);
 assert(io_try_read_int(&r).status==IO_FORMAT);
 assert(io_try_read_ull(&r).status==IO_RANGE);
 assert(io_try_read_uint(&r).status==IO_RANGE);
 DoubleReadResult d=io_try_read_double(&r);assert(d.status==IO_OK && d.value==2.5);
 assert(io_try_read_char(&r,true).value=='\r');assert(io_try_read_char(&r,false).value=='Z');assert(io_try_read_int(&r).status==IO_EOF);fclose(f);
}
