# SPDX-License-Identifier: MIT
"""Build independent public headers from maintained implementation templates."""
import json
import re
from pathlib import Path
ROOT = Path(__file__).resolve().parent

def headers():
    catalog=json.loads((ROOT/'catalog.json').read_text())['modules']
    result={p.name:p.read_text() for p in (ROOT/'include').glob('*.h') if p.name in ('ctb_core.h','ctb_io.h','ctb_math.h','ctb_bits.h')}
    def wrap(name, code, dependencies):
        guard=name.replace('.','_').upper()
        return '#ifndef '+guard+'\n#define '+guard+'\n'+''.join('#include "'+d+'"\n' for d in dependencies)+code+'\n#endif\n'
    def template(module):
        e=catalog[module]
        return '\n'.join((ROOT/'templates'/p).read_text() for p in [e['template']]+e.get('extra_templates',[]))
    result['ctb_string.h']=wrap('ctb_string.h',template('string')+'\n'+(ROOT/'templates/string_io.c').read_text(),['ctb_io.h'])
    for suffix,name,ctype,reader,printer in [('int','IntVector','int','int','int'),('ll','LongLongVector','long long','ll','ll'),('double','DoubleVector','double','double','double')]:
        code=template('vector')
        for p in catalog['vector']['numeric_templates'][ctype]:code+='\n'+(ROOT/'templates'/p).read_text()
        for operation in ('get','pop','front','back'):
            code=re.sub(r'@NAME@_' + operation + r'\b', '@NAME@_' + operation + '_into', code)
        code += '\n' + (ROOT/'templates/vector_public.c').read_text()
        code=code.replace('@NAME@',name).replace('@TYPE@',ctype).replace('@MIN@','LLONG_MIN' if suffix=='ll' else 'INT_MIN').replace('@MAX@','LLONG_MAX' if suffix=='ll' else 'INT_MAX')
        code+=f'\nstatic {name} read_{suffix}s(size_t count) {{ {name} v={name}_new();{name}_reserve(&v,count);for(size_t i=0;i<count;++i) {name}_push(&v,read_{reader}());return v;}}\n'
        code+=f'static void print_{suffix}s(const {name} *v,const char *sep,const char *end) {{for(size_t i=0;i<v->len;++i){{if(i)print_text(sep);print_{printer}(v->data[i]);}}print_text(end);}}\n'
        result[f'ctb_vec_{suffix}.h']=wrap(f'ctb_vec_{suffix}.h',code,['ctb_io.h'])
    for module in ('map','set','counter'):
        dep={'map':'string','set':'map','counter':'map'}[module]
        result[f'ctb_{module}.h']=wrap(f'ctb_{module}.h',template(module),[f'ctb_{dep}.h'])
    return result

if __name__=='__main__':
    for name,code in headers().items():(ROOT/'include'/name).write_text(code)
