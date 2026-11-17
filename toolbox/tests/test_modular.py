# SPDX-License-Identifier: MIT
import os
import random
import shlex
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
            for name,code in headers().items():
                path=folder/name;path.parent.mkdir(parents=True,exist_ok=True);path.write_text(code)
            self.assertNotIn('String', (folder/'ctb/io.h').read_text())
            source='''#include "ctb/string.h"
#include "ctb/vector/int.h"
#include "ctb/vector/double.h"
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
                subprocess.run([cc,'-std=c99',*shlex.split(os.environ.get('TOOLBOX_CFLAGS','')),'-Wall','-Wextra','-Wno-unused-function','-I',tmp,str(path),'-lm','-o',str(exe)],check=True,capture_output=True)
            compile_file(folder/'test.c')
            subprocess.run([str(exe)],input='\n  hello world  \r\n1 2 3\n  raw  \n',text=True,check=True)
            for name in headers():
                (folder/'single.c').write_text('#include "'+name+'"\nint main(void){return 0;}')
                compile_file(folder/'single.c')
    def test_skeleton_submission(self):
        script=ROOT/'dist/c_toolbox_v0.8.1.py'
        with tempfile.TemporaryDirectory() as tmp:
            cases={'blank':'','t':'0\n','eof':'1\n','sentinel':'-1\n','operations':'0\n'}
            for mode,data in cases.items():
                folder=Path(tmp)/mode
                subprocess.run([sys.executable,str(script),'--folder',str(folder),'--skeleton',mode],check=True,capture_output=True)
                self.assertFalse((folder/'c_toolbox.h').exists())
                subprocess.run([sys.executable,str(folder/'prepare.py'),'--no-copy'],check=True,capture_output=True)
                exe=folder/('check.exe' if os.name=='nt' else 'check')
                subprocess.run([os.environ.get('CC','gcc'),'-std=c99',*shlex.split(os.environ.get('TOOLBOX_CFLAGS','')),str(folder/'main.c'),'-lm','-o',str(exe)],check=True,capture_output=True)
                subprocess.run([str(exe)],input=data,text=True,check=True)

    def test_typed_headers_in_prepared_submission(self):
        script = ROOT / 'dist/c_toolbox_v0.8.1.py'
        with tempfile.TemporaryDirectory() as tmp:
            folder = Path(tmp) / 'problem'
            subprocess.run([sys.executable, str(script), '--folder', str(folder)],
                           check=True, capture_output=True)
            cases = [('int', 'Int', 'int', ''),
                     ('long_long', 'LongLong', 'll', ''),
                     ('double', 'Double', 'double', '2, ')]
            source = ['#include <assert.h>', '#include "ctb/string_list.h"']
            for path, _, _, _ in cases:
                for group in ('vector', 'matrix'):
                    source.append(f'#include "ctb/{group}/{path}.h"')
            source.append('int main(void) {')
            for _, name, suffix, precision in cases:
                source.append(f"""
    {{
        {name}Vector v = read_{suffix}s(2);
        {name}Vector_sort_asc(&v);
        assert({name}Vector_get(&v, 0) == 1);
        print_{suffix}s(&v, {precision}" ", "\\n");
        {name}Vector_free(&v);

        {name}Matrix m = read_{suffix}_matrix(1, 2);
        {name}Matrix t = {name}Matrix_transpose(&m);
        {name}Matrix p = {name}Matrix_multiply(&m, &t);
        assert({name}Matrix_get(&p, 0, 0) == 13);
        print_{suffix}_matrix(&p, {precision}" ", "");
        {name}Matrix_free(&p);
        {name}Matrix_free(&t);
        {name}Matrix_free(&m);
    }}""")
            source.append('    return 0;\n}')
            (folder / 'main.c').write_text('\n'.join(source), encoding='utf-8')
            subprocess.run([sys.executable, str(folder / 'prepare.py'), '--no-copy'],
                           check=True, capture_output=True)
            submission = folder / '.toolbox' / 'submit.c'
            exe = folder / ('typed.exe' if os.name == 'nt' else 'typed')
            cc = os.environ.get('CC', 'gcc')
            flags = shlex.split(os.environ.get('TOOLBOX_CFLAGS', ''))
            subprocess.run([cc, '-std=c99', *flags, str(submission), '-lm', '-o', str(exe)],
                           check=True)
            result = subprocess.run([str(exe)], input='2 1 2 3\n' * 3, text=True,
                                    check=True, capture_output=True)
            self.assertEqual(result.stdout, '1 2\n13\n1 2\n13\n1.00 2.00\n13.00\n')

    def test_bigint_against_python(self):
        randomizer=random.Random(70117)
        cases=[(0,1),(1,-1),(-1,1),(10**200-1,10**100+1),(-(10**150),7)]
        for _ in range(300):
            a=randomizer.randrange(-10**randomizer.randrange(1,121),10**randomizer.randrange(1,121))
            b=0
            while not b:b=randomizer.randrange(-10**randomizer.randrange(1,81),10**randomizer.randrange(1,81))
            cases.append((a,b))
        with tempfile.TemporaryDirectory() as tmp:
            exe=Path(tmp)/('bigint.exe' if os.name=='nt' else 'bigint')
            subprocess.run([os.environ.get('CC','gcc'),'-std=c99',*shlex.split(os.environ.get('TOOLBOX_CFLAGS','')),'-I',str(ROOT/'include'),str(ROOT/'tests/modular/bigint.c'),'-lm','-o',str(exe)],check=True)
            result=subprocess.run([str(exe)],input=''.join(str(a)+' '+str(b)+'\n' for a,b in cases),capture_output=True,text=True,check=True)
            lines=result.stdout.splitlines()
            self.assertEqual(len(lines),len(cases))
            for (a,b),line in zip(cases,lines):
                q=abs(a)//abs(b)
                if (a<0)!=(b<0):q=-q
                self.assertEqual(list(map(int,line.split())),[a+b,a-b,a*b,q,a-q*b])

    def test_scalar(self):
        with tempfile.TemporaryDirectory() as tmp:
            exe=Path(tmp)/('test.exe' if os.name=='nt' else 'test')
            for source in ('io.c','algorithms.c','objects.c','custom.c','remaining.c'):
                subprocess.run([os.environ.get('CC','gcc'),'-std=c99',*shlex.split(os.environ.get('TOOLBOX_CFLAGS','')),'-I',str(ROOT/'include'),str(ROOT/'tests/modular'/source),'-lm','-o',str(exe)],check=True)
                subprocess.run([str(exe)],check=True)
if __name__=='__main__':unittest.main()
