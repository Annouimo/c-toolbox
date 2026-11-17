# SPDX-License-Identifier: MIT
"""Build independent public headers from maintained implementation templates."""
import json
import re
from pathlib import Path
ROOT = Path(__file__).resolve().parent

def headers():
    catalog=json.loads((ROOT/'catalog.json').read_text(encoding="utf-8"))['modules']
    result={p.name:p.read_text(encoding="utf-8") for p in (ROOT/'include').glob('*.h') if p.name in ('ctb_core.h','ctb_io.h','ctb_math.h','ctb_bits.h','ctb_array.h','ctb_prefix_sum.h','ctb_matrix.h','ctb_date.h','ctb_bigint.h','ctb_number.h','ctb_sieve.h','ctb_search.h','ctb_complex.h','ctb_combination.h')}
    def wrap(name, code, dependencies):
        guard=name.replace('.','_').upper()
        return '#ifndef '+guard+'\n#define '+guard+'\n'+''.join('#include "'+d+'"\n' for d in dependencies)+code+'\n#endif\n'
    def template(module):
        e=catalog[module]
        return '\n'.join((ROOT/'templates'/p).read_text(encoding="utf-8") for p in [e['template']]+e.get('extra_templates',[]))
    string_code=template('string').replace('string_list_pop(', 'string_list_pop_into(')
    string_code+='\n'+(ROOT/'templates/string_io.c').read_text(encoding='utf-8')+'\n'+(ROOT/'templates/string_public.c').read_text(encoding='utf-8')
    result['ctb_string.h']=wrap('ctb_string.h',string_code,['ctb_io.h'])
    for suffix,name,ctype,reader,printer in [('int','IntVector','int','int','int'),('ll','LongLongVector','long long','ll','ll'),('double','DoubleVector','double','double','double'),('size','IndexVector','size_t','ull','ull')]:
        code=template('vector')
        for p in catalog['vector']['numeric_templates'].get(ctype, []):code+='\n'+(ROOT/'templates'/p).read_text(encoding="utf-8")
        for operation in ('get','pop','front','back'):
            code=re.sub(r'@NAME@_' + operation + r'\b', '@NAME@_' + operation + '_into', code)
        code += '\n' + (ROOT/'templates/vector_public.c').read_text(encoding="utf-8")
        code=code.replace('@NAME@',name).replace('@TYPE@',ctype).replace('@MIN@','LLONG_MIN' if suffix=='ll' else 'INT_MIN').replace('@MAX@','LLONG_MAX' if suffix=='ll' else 'INT_MAX')
        code+=f'\nstatic {name} read_{suffix}s(size_t count) {{ {name} v={name}_new();{name}_reserve(&v,count);for(size_t i=0;i<count;++i) {name}_push(&v,read_{reader}());return v;}}\n'
        code+=f'static void print_{suffix}s(const {name} *v,const char *sep,const char *end) {{for(size_t i=0;i<v->len;++i){{if(i)print_text(sep);print_{printer}(v->data[i]);}}print_text(end);}}\n'
        result[f'ctb_vec_{suffix}.h']=wrap(f'ctb_vec_{suffix}.h',code,['ctb_io.h'])
    for module in ('map','set','counter'):
        dep={'map':'string','set':'map','counter':'map'}[module]
        code=template(module)
        for operation in ('map_get','map_get_s','map_next','counter_next','counter_most_common','counter_most_common_string'):
            code=re.sub(r'\b'+operation+r'\b',operation+'_into',code)
        if module in ('map','counter'):
            code+='\n'+(ROOT/'templates'/(module+'_public.c')).read_text(encoding='utf-8')
        result[f'ctb_{module}.h']=wrap(f'ctb_{module}.h',code,[f'ctb_{dep}.h'])
    # Explicit instantiation only: merely including these headers defines no containers.
    for kind in ('vector','deque','heap'):
        code=template(kind)
        if kind=='vector':
            code=re.sub(r'#define @NAME@_values[^\n]*\n[^\n]*\n','',code)
            for operation in ('get','pop','front','back'):
                code=re.sub(r'@NAME@_'+operation+r'\b','@NAME@_'+operation+'_into',code)
            code+='\n'+(ROOT/'templates/vector_public.c').read_text(encoding='utf-8')
        else:
            operations=('get','front','back','pop_front','pop_back') if kind=='deque' else ('peek','pop')
            for operation in operations:
                code=re.sub(r'@NAME@_'+operation+r'\b','@NAME@_'+operation+'_into',code)
            code+='\ntypedef struct { bool ok; @TYPE@ value; } @NAME@Result;\n'
            for operation in operations:
                arg=',size_t pos' if operation=='get' else ''
                call=',pos' if operation=='get' else ''
                const='' if operation.startswith('pop') else 'const '
                code+=f'static @NAME@Result @NAME@_try_{operation}({const}@NAME@ *v{arg}) {{ @NAME@Result r={{false,(@TYPE@){{0}}}};r.ok=@NAME@_{operation}_into(v{call},&r.value);return r;}}\n'
                code+=f'static @TYPE@ @NAME@_{operation}({const}@NAME@ *v{arg}) {{ @NAME@Result r=@NAME@_try_{operation}(v{call});if(!r.ok)tb_fail("invalid container access");return r.value;}}\n'
        code=re.sub(r'/\*.*?\*/','',code,flags=re.S)
        code=code.replace('@TYPE@','Type')
        code=re.sub(r'@NAME@([A-Za-z0-9_]*)',lambda m:'CTB_JOIN(Name,'+m[1]+')' if m[1] else 'Name',code)
        # Use a literal backslash-newline separator in generated macro definitions.
        body=(' '+chr(92)+'\n').join(line.strip() for line in code.splitlines() if line.strip())
        common='#ifndef CTB_JOIN\n#define CTB_JOIN_RAW(a,b) a##b\n#define CTB_JOIN(a,b) CTB_JOIN_RAW(a,b)\n#endif\n'
        result[f'ctb_{kind}.h']=wrap(f'ctb_{kind}.h',common+f'#define CTB_{kind.upper()}_DEFINE(Name,Type) '+chr(92)+'\n'+body+'\n',['ctb_core.h'])
    return result

if __name__=='__main__':
    for name,code in headers().items():(ROOT/'include'/name).write_text(code, encoding="utf-8")
