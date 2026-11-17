/* Exact unsigned accumulation; fails before wraparound. */
static unsigned long long @NAME@_sum(const @NAME@ *v) {
    unsigned long long result=0;
    for(size_t i=0;i<v->len;++i) {
        unsigned long long value=(unsigned long long)v->data[i];
        if(result>ULLONG_MAX-value)tb_fail("unsigned vector sum overflow");
        result+=value;
    }
    return result;
}
