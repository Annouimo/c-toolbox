# C Toolbox 速查 · v@VERSION@

设计者：Annouimo。C99，面向已有编程经验的用户。所有位置按字节或元素计数。

## 创建与提交

运行入口 Python 脚本，输入题目目录并选择输入骨架；留空仅在入口旁导出本手册。新题目包含 main.c、模块头文件、prepare.py 和手册；已有目录不覆盖。默认 main.c 仅引入 ctb_io.h，按需增加头文件。各模块自动包含依赖。

运行同目录 prepare.py：单个 C 文件自动选中，多个文件询问选择；展开本地双引号 include，保留系统头文件，编译检查后复制。源码保存到 .toolbox/submit.c，不覆盖原文件。--no-copy 只保存检查，--run 显式运行，--cc 指定编译器。无编译器或编译失败不复制。手动编译：cc -std=c99 -O2 main.c -o main -lm。

每题保存创建时版本，更新入口不修改旧题。提交展开不做函数裁剪；用不到的模块不要引入。

## 头文件选择

| 头文件 | 能力 |
| --- | --- |
| ctb_io.h | 内置标量、字符、基础输出、输入流控制 |
| ctb_string.h | String、StringList 及读写 |
| ctb_vec_int.h / ctb_vec_ll.h / ctb_vec_double.h | IntVector / LongLongVector / DoubleVector 及读写 |
| ctb_vec_size.h | IndexVector，索引存储 |
| ctb_vector.h / ctb_deque.h / ctb_heap.h | 自定义类型容器的显式声明宏，包含本身不实例化 |
| ctb_map.h / ctb_set.h / ctb_counter.h | 字符串键映射、集合、频次；Counter 模块同时提供 IntCounter |
| ctb_bits.h | 位读写与计数 |
| ctb_math.h | GCD/LCM、阶乘、幂、舍入、浮点比较、判素数、多项式 |
| ctb_number.h | 进制转换、数位 |
| ctb_sieve.h | 素数筛 |
| ctb_array.h / ctb_prefix_sum.h | 整数向量算法、前缀和 |
| ctb_search.h | 整数答案二分、数值二分求根 |
| ctb_combination.h | 组合索引迭代 |
| ctb_complex.h | C99 复数封装 |
| ctb_matrix.h | double 矩阵 |
| ctb_date.h | 公历日期、基础时刻 |
| ctb_bigint.h | 有符号任意精度整数 |

## 输入输出

| 任务 | 接口 |
| --- | --- |
| 标量 | read_int / read_ll / read_uint / read_ull / read_double，无参数、返回值 |
| 检查读取 | try_read_int() 等，返回 status/value；status 与 IO_OK、IO_EOF、IO_FORMAT、IO_RANGE、IO_STREAM_ERROR 比较 |
| 字符 | read_char() 跳过空白；read_char_raw() 保留空白 |
| 字符串 | read_word / read_line / read_line_raw / read_rest / read_next_line / read_next_line_raw，返回 String |
| 字符串检查版 | try_read_word() 等返回 status/value，成功对象由调用者释放 |
| 批量读取 | read_ints / read_lls / read_doubles / read_words(count)，随类型模块提供 |
| 输出 | print_int(x)、print_ll(x)、print_double(x)、print_text("YES")、print_char(ch)、print_newline() |
| 小数与格式 | print_fixed(x,digits)（消除输出负零）、print_format(format,...)，格式串遵循 printf |
| 容器输出 | print_ints / print_lls / print_doubles(&v,separator,end)；print_strings(&list,separator,end) |
| 字符串输出 | print_string(&s)，不自动加换行 |
| 输入控制 | io_skip_line() 丢弃行尾；io_skip_whitespace() 跳过空白 |
| 文件 | IOReader r=io_reader(file)；io_try_read_int(&r)、io_try_read_line_raw(&r) 等 |

普通数值读取按完整单词解析，不把 12abc 当成 12；失败报告错误并退出。检查接口不退出，失败时 value 仅作占位，必须先检查状态；IO_OK=0，不能当布尔值使用。同一流不混用 scanf 等其他读取器。

line 默认跳过空白行、修剪首尾，保留内部空格；raw 保留空行与首尾空白，移除 LF/CRLF/CR。数字后 raw 读取当前分隔符起的行尾；next_line_raw 跨过数字所在行再读下一完整行，遇到额外非空白数据返回 IO_EXTRA_DATA。read_rest 读取当前行剩余内容。

## String 与 StringList

| 任务 | 接口 |
| --- | --- |
| 创建、释放 | string_new / string_from(text) / string_clone(&s)；string_free(&s) |
| 查询 | string_size/empty/compare/equal；string_cstr(&s) 返回借用的 const char*；string_at(&s,i) |
| 修改 | string_assign/append/insert/erase；_s 后缀接 String；trim/lower/upper/reverse |
| 切片 | string_slice(&s,pos,count)，返回新对象 |
| 查找 | string_find(&s,needle,start)，未找到 SIZE_MAX；contains/count/count_overlapping |
| 替换 | string_replace_all；remove_first/remove_all/remove_repeated，repeated 会重查删除后形成的新匹配 |
| 分割、连接 | string_split(&s,delimiter) 保留空字段；string_split_whitespace；string_join(&list,separator) |
| 格式化 | string_format(format,...)；string_append_format(&s,format,...) |
| 连续字符 | string_longest_char_run(&s,ch) 返回 begin/end，同长取最早 |
| 列表 | string_list_new/push/push_text/push_take/get/at/set/insert/erase/clear/clone/slice/free |
| 列表弹出 | string_list_pop(&list) 返回拥有资源的 String；string_list_try_pop 返回 ok/value |
| 列表排序 | string_list_sort；sort_by；reverse；unique_sorted |

StringList 默认深拷贝；push_take 移走对象并清空源对象；get/at 返回借用指针，列表改变后可能失效。文本按字节处理，不支持内嵌 NUL，不承诺 Unicode 字符索引。

## Vector、自定义容器

以下 _ 表示对应类型前缀，如 IntVector_push。内置数值容器按需引入，不自动生成其他类型。

| 任务 | 接口 |
| --- | --- |
| 创建 | _new()、_from(items,count)、_values(1,2,3)；整数 _range(start,stop,step) 不含 stop |
| 修改 | _push、_insert、_erase、_set、_resize、_fill、_reverse、_clear、_free |
| 取值 | _get(&v,i)、_at、_front、_back、_pop；检查版 _try_get / _try_pop 返回 ok/value |
| 复制 | _clone、_slice(&v,pos,count)、_extend_vector，返回新对象的需释放 |
| 排序 | 数值 _sort_asc/desc；_sort_by 使用类型明确的比较器、稳定排序；_sort 使用 qsort 形比较器 |
| 查询 | _find/contains/count；_lower_bound/upper_bound 要求升序；_unique_sorted |
| 统计 | _sum/mean/variance/stddev/median/min/max/argmin/argmax |
| 函数操作 | _map/filter/reduce、any/all/count_if/find_if/map_inplace，使用普通 C 函数回调 |

find 未找到返回 SIZE_MAX。slice 是起点＋数量，不支持负下标或步长。空容器 sum=0，其余统计要求非空；variance 为总体方差；median 不修改输入；整数 sum 检查每步 long long 溢出。容器元素浅拷贝，拥有字符串的序列优先 StringList。

自定义类型：先定义元素类型，再 CTB_VECTOR_DEFINE(Name,Type)、CTB_DEQUE_DEFINE(Name,Type) 或 CTB_HEAP_DEFINE(Name,Type)。这些宏不生成数值统计或 values 宏；用 _from 或 push 创建。类型名必须互不冲突且不要与内置容器重复实例化。

Deque 提供 push_front/back、pop_front/back、get/front/back；检查版 try_ 前缀返回 ok/value。Heap 通过 new(compare) 设置 qsort 形比较器，负数表示第一个元素优先；push、peek、pop，检查版 try_peek/try_pop。队列和堆均提供 size/empty/clear/clone/free，元素浅拷贝。

## Map、Set、Counter

Map 字符串键自持有，值 long long：new/set/get/get_or/contains/remove/keys/size/clear/clone/free。map_get 返回值，缺失退出；map_try_get 返回 ok/value；_s 接 String。Set 提供 add/contains/remove、union/intersection/difference/is_subset。

Counter：add/get/remove/keys/from_strings；缺失值为 0，支持负计数，add 溢出返回 false 且不修改。most_common 返回 ok/key/count，key 借用；most_common_string 返回拥有资源的 String key，需释放；并列选字典序最小键。IntCounter 提供 int_counter_new/add/get/free，键为 long long，内部复用十进制字符串哈希。迭代顺序不保证。

少数低层兼容接口以 _into 明确表达输出指针，优先使用返回值接口。

## 数学与整数

| 任务 | 接口与约定 |
| --- | --- |
| 位 | bit_get/set/clear/toggle(value,bit)，最低位 0，返回结果；bit_count，unsigned long long |
| GCD/LCM | gcd/lcm(a,b)，输入 long long，返回 unsigned long long；负数取绝对值，gcd(0,0)=0 |
| 阶乘、幂 | factorial(unsigned n) 返回 unsigned long long；integer_power(base,unsigned exponent) 返回 long long，0^0=1 |
| 检查计算 | try_factorial/try_lcm/try_integer_power 返回 ok/value；普通版溢出退出 |
| 舍入 | round_digits(x,digits)，中点远离零，负 digits 表示十/百位；受浮点表示精度影响 |
| 浮点比较 | nearly_equal(a,b,absolute_tolerance,relative_tolerance) |
| 多项式 | polynomial_value(coefficients,count,x)，系数按次数升序 |
| 进制 | integer_to_base(value,base) 返回 String；integer_from_base(&s,base)，检查版 try_integer_from_base，base 2～36 |
| 数位 | integer_digit_count/sum(value,base)，负数按绝对值计算 |
| 素数 | is_prime(n) 试除；prime_sieve_new(limit)/contains/free；查询不能超出筛范围 |
| 答案二分 | integer_first_true(begin,end,predicate)，闭区间、假到真；返回 ok/value |
| 二分求根 | bisect_root(function,begin,end,tolerance,iterations)，返回 ok/value；连续函数且端点异号，不保证检测不连续点 |

## 数组与区间

IntVector_longest_equal_run 返回 begin/end；IntVector_max_subarray 返回 ok/sum/begin/end，要求非空子段，空输入 ok=false。区间半开，同分取最早。IntVector_difference 返回 LongLongVector，避免 int 相减溢出。

IntVector_argsort 返回 IndexVector，存升序排列的原下标，同值保留原顺序。IntVector_discretize 返回 values（排序去重值）和 ranks（原序零基排名），使用 int_discretization_free 释放两者。

IntVector_next_permutation 原地推进，末尾重置升序并返回 false。Combination c=combination_new(n,k)；while(combination_next(&c)) 使用 c.indices；k=0 产生一个空组合；combination_free 释放。

PrefixSum p=prefix_sum_from(&v) 保存 IntVector 快照；prefix_sum_query(&p,begin,end)；prefix_sum_free。修改原向量不更新快照，溢出或无效查询退出。不提供 ST/RMQ。

## 矩阵、复数、日期

Matrix 保存 double：matrix_new(rows,cols)、get/set、clone、transpose、multiply；每个新结果 matrix_free。read_matrix(rows,cols)；print_matrix(&m,digits,separator) 每行换行。乘法要求维度匹配，不承诺整数精确性。

Complex 是 C99 double complex：complex_new(real,imaginary)、real/imaginary/magnitude、add/subtract/multiply/divide；read_complex 读实虚两数，print_complex(z,digits)。值类型无须释放，除零退出。

Date 为公历 1～9999 年值类型：date_new/try_date/valid/is_leap/compare；date_difference(a,b) 为从 a 到 b 的天数；date_add_days/try_date_add_days。read_date 读三个空白分隔整数；print_date 输出 YYYY-MM-DD。TimeOfDay 与 time_from_seconds/time_to_seconds 限一天内，time_difference(a,b) 为同一天内有符号秒差；read_time 读三个整数，print_time 输出 HH:MM:SS，无时区、夏令时。

## 大整数

BigInt：bigint_new/from("-123")/from_ll/clone/compare/negate/free；try_bigint_from 返回 ok/value。add/subtract/multiply/divide/modulo 返回新对象；divmod 返回 quotient/remainder，两者都释放；multiply_small、divide_small（返回商余数）；factorial。read_bigint/try_read_bigint、print_bigint、bigint_to_string。

解析仅接受可选正负号和至少一个十进制数字。除法向零截断，余数与被除数同号，满足 a=q*b+r；除零退出。所有新算术结果独立拥有内存。采用十进制学校算法，适合普通习题，非百万位高性能库。

## 所有权与错误

对象先初始化；拥有内存的结构体赋值不自动深拷贝，独立副本用 clone。返回新对象的调用由使用者释放，修改已有对象用指针。分配失败、非法必需参数、普通读取失败会报错退出；需要自行处理的情况使用检查接口并先检查状态。字符串和容器的借用指针可能在修改后失效。
