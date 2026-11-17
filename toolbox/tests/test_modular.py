# SPDX-License-Identifier: MIT
import os
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT))
from build_headers import headers
class ModularTests(unittest.TestCase):
    def test_headers_and_io(self):
        with tempfile.TemporaryDirectory() as tmp:
            folder=Path(tmp)
            for name,code in headers().items(): (folder/name).write_text(code)
            self.assertNotIn('String', (folder/'ctb_io.h').read_text())
            source='''#include "ctb_string.h"
#include "ctb_vec_int.h"
#include "ctb_vec_double.h"
#include "ctb_counter.h"
#include <assert.h>
int main(void){
 StringReadResult a=try_read_line_raw(); assert(a.status==IO_OK && !a.value.len);string_free(&a.value);
 String s=read_line(); assert(!strcmp(string_cstr(&s),"hello world"));string_free(&s);
 IntVector v=read_ints(3);assert(IntVector_sum(&v)==6);IntVector_free(&v);
 io_skip_line();s=read_line_raw();assert(!strcmp(string_cstr(&s),"  raw  "));string_free(&s);
 assert(try_read_int().status==IO_EOF);return 0;
}'''
            (folder/'test.c').write_text(source)
            exe=folder/('test.exe' if os.name=='nt' else 'test')
            cc=os.environ.get('CC','gcc')
            def compile_file(path):
                subprocess.run([cc,'-std=c99','-Wall','-Wextra','-Wno-unused-function','-I',tmp,str(path),'-lm','-o',str(exe)],check=True,capture_output=True)
            compile_file(folder/'test.c')
            subprocess.run([str(exe)],input='\n  hello world  \r\n1 2 3\n  raw  \n',text=True,check=True)
            for name in headers():
                (folder/'single.c').write_text('#include "'+name+'"\nint main(void){return 0;}')
                compile_file(folder/'single.c')
    def test_skeleton_submission(self):
        script=ROOT/'dist/c_toolbox_v0.5.0.py'
        with tempfile.TemporaryDirectory() as tmp:
            cases={'blank':'','t':'0\n','eof':'1 2\n','eof_pair':'1 2\n','sentinel':'-1\n','operations':'0\n','eof_word':'a b\n','eof_line':'a\nb\n','eof_line_raw':'\n a \n'}
            for mode,data in cases.items():
                folder=Path(tmp)/mode
                subprocess.run([sys.executable,str(script),'--folder',str(folder),'--skeleton',mode],check=True,capture_output=True)
                self.assertFalse((folder/'c_toolbox.h').exists())
                subprocess.run([sys.executable,str(folder/'prepare.py'),'--no-copy'],check=True,capture_output=True)
                exe=folder/('check.exe' if os.name=='nt' else 'check')
                subprocess.run([os.environ.get('CC','gcc'),'-std=c99',str(folder/'main.c'),'-lm','-o',str(exe)],check=True,capture_output=True)
                subprocess.run([str(exe)],input=data,text=True,check=True)

    def test_scalar(self):
        with tempfile.TemporaryDirectory() as tmp:
            exe=Path(tmp)/('test.exe' if os.name=='nt' else 'test')
            subprocess.run([os.environ.get('CC','gcc'),'-std=c99','-I',str(ROOT/'include'),str(ROOT/'tests/modular/io.c'),'-o',str(exe)],check=True)
            subprocess.run([str(exe)],check=True)
if __name__=='__main__':unittest.main()
