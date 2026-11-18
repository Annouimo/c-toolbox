# SPDX-License-Identifier: MIT
"""Build independent public headers from maintained implementation templates."""
import json
import re
from pathlib import Path
ROOT = Path(__file__).resolve().parent

def headers():
    catalog=json.loads((ROOT/'catalog.json').read_text(encoding="utf-8"))['modules']
    maintained = (
        'detail/core.h', 'io.h', 'math/arithmetic.h', 'math/bits.h',
        'sequence/algorithms.h', 'sequence/prefix_sum.h', 'sequence/range_difference.h',
        'date.h', 'math/bigint.h', 'math/number.h', 'math/sieve.h', 'search.h',
        'math/complex.h', 'sequence/combination.h',
    )
    result = {'ctb/' + name: (ROOT / 'include/ctb' / name).read_text(encoding='utf-8')
              for name in maintained}
    def wrap(name, code, dependencies):
        guard=re.sub(r'[^A-Za-z0-9_]','_',name).upper()
        return '#ifndef '+guard+'\n#define '+guard+'\n'+''.join('#include "'+d+'"\n' for d in dependencies)+re.sub(r"[ \t]+$", "", code, flags=re.M)+'\n#endif\n'
    def template(module):
        e=catalog[module]
        return '\n'.join((ROOT/'templates'/p).read_text(encoding="utf-8") for p in [e['template']]+e.get('extra_templates',[]))
    def read_template(name):
        return (ROOT / 'templates' / name).read_text(encoding='utf-8')

    matrix_base = read_template('matrix_base.c')
    matrix_numeric = read_template('matrix_typed.c')
    matrix_print = read_template('matrix_print.c')
    for suffix, filename, name, ctype in (
        ('int', 'int', 'IntMatrix', 'int'),
        ('ll', 'long_long', 'LongLongMatrix', 'long long'),
        ('double', 'double', 'DoubleMatrix', 'double'),
    ):
        integer = suffix != 'double'
        code = matrix_base + matrix_numeric + matrix_print
        code = (code.replace('@NAME@', name).replace('@TYPE@', ctype)
                .replace('@SUFFIX@', suffix).replace('@READ@', f'read_{suffix}')
                .replace('@ACCUMULATOR@', 'long long' if integer else 'double')
                .replace('@ACCUMULATE@', 'total = tb_matrix_add_product(total, x, y);'
                         if integer else 'total += x * y;')
                .replace('@SUM@', 'total = tb_matrix_checked_add(total, value, "matrix sum overflow");'
                         if integer else 'total += value;')
                .replace('@CHECK@', 'if (total < INT_MIN || total > INT_MAX)'
                         ' tb_fail("matrix multiplication overflow");' if suffix == 'int' else '')
                .replace('@PRECISION@', ', int digits' if suffix == 'double' else '')
                .replace('@PRINT@', 'print_fixed(value, digits, "")' if suffix == 'double'
                         else f'print_{suffix}(value, "")'))
        path = f'ctb/matrix/{filename}.h'
        deps = ['../io.h', 'detail/border.h']
        if integer:
            deps.append('detail/integer.h')
        result[path] = wrap(path, code, deps)
    path = 'ctb/matrix/detail/integer.h'
    result[path] = wrap(path, read_template('matrix_integer.c'), ['../../math/arithmetic.h'])

    path = 'ctb/matrix/detail/border.h'
    result[path] = wrap(path, read_template('matrix_border.c'), ['../../detail/core.h'])
    for ctype, name in (('char', 'CharMatrix'), ('bool', 'BoolMatrix')):
        path = f'ctb/matrix/{ctype}.h'
        code = matrix_base.replace('@NAME@', name).replace('@TYPE@', ctype)
        printer = 'print_char(value, "")' if ctype == 'char' else 'print_int(value, "")'
        code += (matrix_print.replace('@NAME@', name).replace('@TYPE@', ctype)
                 .replace('@SUFFIX@', ctype).replace('@PRECISION@', '').replace('@PRINT@', printer))
        deps = ['detail/border.h', '../io.h']
        if ctype == 'char':
            code += read_template('matrix_char_io.c')
            deps.append('../string.h')
        result[path] = wrap(path, code, deps)

    string_files = ('string.c', 'string_extra.c', 'string_format.c', 'string_objects.c',
                    'string_io.c', 'string_public.c')
    list_files = ('string_list_base.c', 'string_list.c', 'string_list_objects.c',
                  'string_list_argsort.c', 'string_list_io.c', 'string_list_public.c')
    string_code = '\n'.join(map(read_template, string_files))
    list_code = '\n'.join(map(read_template, list_files))
    list_code = list_code.replace('static bool string_list_pop(', 'static bool string_list_pop_into(')
    result['ctb/string.h'] = wrap('ctb/string.h', string_code, ['io.h'])
    result['ctb/string_list.h'] = wrap('ctb/string_list.h', list_code, ['string.h', 'vector/index.h'])
    for suffix,name,ctype,reader,printer in [('int','IntVector','int','int','int'),('ll','LongLongVector','long long','ll','ll'),('double','DoubleVector','double','double','double'),('size','IndexVector','size_t','ull','ull'),('char','CharVector','char','char','char'),('bool','BoolVector','bool',None,None)]:
        code=template('vector')
        for p in catalog['vector']['numeric_templates'].get(ctype, []):code+='\n'+(ROOT/'templates'/p).read_text(encoding="utf-8")
        for operation in ('get','pop','front','back'):
            code=re.sub(r'@NAME@_' + operation + r'\b', '@NAME@_' + operation + '_into', code)
        code = code.replace('@NAME@_set(', '@NAME@_try_set(')
        code += '\n' + (ROOT/'templates/vector_public.c').read_text(encoding="utf-8")
        code=code.replace('@NAME@',name).replace('@TYPE@',ctype).replace('@MIN@','LLONG_MIN' if suffix=='ll' else 'INT_MIN').replace('@MAX@','LLONG_MAX' if suffix=='ll' else 'INT_MAX')
        if reader is not None:
            code+=f'\nstatic {name} read_{suffix}s(size_t count) {{ {name} v={name}_new();{name}_reserve(&v,count);for(size_t i=0;i<count;++i) {name}_push(&v,read_{reader}());return v;}}\n'
            if suffix=='double':
                code+=f'\nstatic void print_doubles(const {name} *v,int digits,const char *sep,const char *end) {{for(size_t i=0;i<v->len;++i){{if(i)print_text(sep,"");print_double(v->data[i],digits,"");}}print_text("",end);}}\n'
            else:
                code+=f'\nstatic void print_{suffix}s(const {name} *v,const char *sep,const char *end) {{for(size_t i=0;i<v->len;++i){{if(i)print_text(sep,"");print_{printer}(v->data[i],"");}}print_text("",end);}}\n'
        if suffix == 'bool':
            code += """
static void print_bools(const BoolVector *v, const char *sep, const char *end) {
    for (size_t i = 0; i < v->len; ++i) {
        if (i) print_text(sep, "");
        print_int(v->data[i], "");
    }
    print_text("", end);
}
"""
        filename = {'ll': 'long_long', 'size': 'index'}.get(suffix, suffix)
        path = f'ctb/vector/{filename}.h'
        result[path] = wrap(path, code, ['../io.h'])

    custom_vector = re.sub(r'#define @NAME@_values[^\n]*\n[^\n]*\n', '', template('vector'))
    for operation in ('get', 'pop', 'front', 'back'):
        custom_vector = re.sub(r'@NAME@_' + operation + r'\b', '@NAME@_' + operation + '_into', custom_vector)
    custom_vector = custom_vector.replace('@NAME@_set(', '@NAME@_try_set(')
    custom_vector += '\n' + read_template('vector_public.c')
    for kind, code in (('vector', custom_vector), ('matrix', matrix_base)):
        code=re.sub(r'/\*.*?\*/','',code,flags=re.S)
        code=code.replace('@TYPE@','Type')
        code=re.sub(r'@NAME@([A-Za-z0-9_]*)',lambda m:'CTB_JOIN(Name,'+m[1]+')' if m[1] else 'Name',code)
        # Use a literal backslash-newline separator in generated macro definitions.
        body=(' '+chr(92)+'\n').join(line.strip() for line in code.splitlines() if line.strip())
        common='#ifndef CTB_JOIN\n#define CTB_JOIN_RAW(a,b) a##b\n#define CTB_JOIN(a,b) CTB_JOIN_RAW(a,b)\n#endif\n'
        result[f'ctb/{kind}/custom.h']=wrap(f'ctb/{kind}/custom.h',common+f'#define CTB_{kind.upper()}_DEFINE(Name,Type) '+chr(92)+'\n'+body+'\n',
            ['detail/border.h'] if kind == 'matrix' else ['../detail/core.h'])
    return result

if __name__=='__main__':
    for name,code in headers().items():
        path=ROOT/'include'/name
        path.parent.mkdir(parents=True,exist_ok=True)
        path.write_text(code, encoding="utf-8")
