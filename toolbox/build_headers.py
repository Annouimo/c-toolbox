# SPDX-License-Identifier: MIT
"""Build independent public headers from maintained implementation templates."""
import json
import re
from pathlib import Path
ROOT = Path(__file__).resolve().parent

def headers():
    catalog=json.loads((ROOT/'catalog.json').read_text(encoding="utf-8"))['modules']
    result={('ctb/'+p.name):p.read_text(encoding="utf-8") for p in (ROOT/'include'/'ctb').glob('*.h') if p.name in ('core.h','io.h','math.h','bits.h','array.h','prefix_sum.h','range_difference.h','matrix.h','matrix_double.h','matrix_int.h','matrix_ll.h','numeric_map.h','byte_vector.h','date.h','bigint.h','number.h','sieve.h','search.h','complex.h','combination.h')}
    def wrap(name, code, dependencies):
        guard=re.sub(r'[^A-Za-z0-9_]','_',name).upper()
        return '#ifndef '+guard+'\n#define '+guard+'\n'+''.join('#include "'+d+'"\n' for d in dependencies)+code+'\n#endif\n'
    def template(module):
        e=catalog[module]
        return '\n'.join((ROOT/'templates'/p).read_text(encoding="utf-8") for p in [e['template']]+e.get('extra_templates',[]))
    typed_matrix=(ROOT/'templates/matrix_typed.c').read_text(encoding='utf-8')
    for suffix,name,ctype,reader,printer,mul,check in [
        ('int','IntMatrix','int','read_int','print_int','product=(long long)a->data[i*a->cols+k]*(long long)b->data[k*b->cols+j];','if(total<INT_MIN || total>INT_MAX)tb_fail("matrix multiplication overflow");'),
        ('ll','LongLongMatrix','long long','read_ll','print_ll','if(a->data[i*a->cols+k] && b->data[k*b->cols+j]) { long long x=a->data[i*a->cols+k],y=b->data[k*b->cols+j]; if((x==-1 && y==LLONG_MIN)||(y==-1 && x==LLONG_MIN)||(x>0 ? (y>0 ? x>LLONG_MAX/y : y<LLONG_MIN/x) : (y>0 ? x<LLONG_MIN/y : x<LLONG_MAX/y)))tb_fail("matrix multiplication overflow"); product=x*y; } else product=0;','')]:
        code=typed_matrix.replace('@NAME@',name).replace('@TYPE@',ctype).replace('@READ@',reader).replace('@PRINT@',printer).replace('@SUFFIX@',suffix).replace('@MUL@',mul).replace('@CHECK@',check)
        result[f'ctb/matrix_{suffix}.h']=wrap(f'ctb/matrix_{suffix}.h',code,['io.h'])
    string_code=template('string').replace('string_list_pop(', 'string_list_pop_into(')
    string_code+='\n'+(ROOT/'templates/string_list_argsort.c').read_text(encoding='utf-8')+'\n'+(ROOT/'templates/string_io.c').read_text(encoding='utf-8')+'\n'+(ROOT/'templates/string_public.c').read_text(encoding='utf-8')
    result['ctb/string.h']=wrap('ctb/string.h',string_code,['io.h','vec_size.h'])
    for suffix,name,ctype,reader,printer in [('int','IntVector','int','int','int'),('ll','LongLongVector','long long','ll','ll'),('double','DoubleVector','double','double','double'),('uint','UIntVector','unsigned int','uint','uint'),('ull','ULongLongVector','unsigned long long','ull','ull'),('size','IndexVector','size_t','ull','ull')]:
        code=template('vector')
        for p in catalog['vector']['numeric_templates'].get(ctype, []):code+='\n'+(ROOT/'templates'/p).read_text(encoding="utf-8")
        for operation in ('get','pop','front','back'):
            code=re.sub(r'@NAME@_' + operation + r'\b', '@NAME@_' + operation + '_into', code)
        code += '\n' + (ROOT/'templates/vector_public.c').read_text(encoding="utf-8")
        code=code.replace('@NAME@',name).replace('@TYPE@',ctype).replace('@MIN@','LLONG_MIN' if suffix=='ll' else 'INT_MIN').replace('@MAX@','LLONG_MAX' if suffix=='ll' else 'INT_MAX')
        code+=f'\nstatic {name} read_{suffix}s(size_t count) {{ {name} v={name}_new();{name}_reserve(&v,count);for(size_t i=0;i<count;++i) {name}_push(&v,read_{reader}());return v;}}\n'
        if suffix=='double':
            code+=f'\nstatic void print_doubles(const {name} *v,int digits,const char *sep,const char *end) {{for(size_t i=0;i<v->len;++i){{if(i)print_text(sep,"");print_double(v->data[i],digits,"");}}print_text("",end);}}\n'
        else:
            code+=f'\nstatic void print_{suffix}s(const {name} *v,const char *sep,const char *end) {{for(size_t i=0;i<v->len;++i){{if(i)print_text(sep,"");print_{printer}(v->data[i],"");}}print_text("",end);}}\n'
        result[f'ctb/vec_{suffix}.h']=wrap(f'ctb/vec_{suffix}.h',code,['io.h'])
    for module in ('map','set','counter'):
        dep={'map':'string','set':'map','counter':'map'}[module]
        code=template(module)
        for operation in ('map_get','map_get_s','map_next','counter_next','counter_most_common','counter_most_common_string'):
            code=re.sub(r'\b'+operation+r'\b',operation+'_into',code)
        if module in ('map','counter'):
            code+='\n'+(ROOT/'templates'/(module+'_public.c')).read_text(encoding='utf-8')
        result[f'ctb/{module}.h']=wrap(f'ctb/{module}.h',code,[{'map':'string.h','set':'map.h','counter':'map.h'}[module]])
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
        result[f'ctb/{kind}.h']=wrap(f'ctb/{kind}.h',common+f'#define CTB_{kind.upper()}_DEFINE(Name,Type) '+chr(92)+'\n'+body+'\n',['core.h'])
    return result

if __name__=='__main__':
    for name,code in headers().items():
        path=ROOT/'include'/name
        path.parent.mkdir(parents=True,exist_ok=True)
        path.write_text(code, encoding="utf-8")
