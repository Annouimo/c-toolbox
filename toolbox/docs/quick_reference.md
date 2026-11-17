> 模块化迁移开发版：以下容器接口仍有部分旧式输出指针，后续统一；旧生成器暂留内部回归测试。

# C Toolbox 速查 · v@VERSION@

设计者：Annouimo。面向熟悉高级语言、需要用 C99 解题的使用者。只列常用公开接口；Ctrl+F 搜索任务或函数名。

## 开始与提交

运行入口脚本，填写题目文件夹并选择输入骨架；支持子目录，路径相对入口脚本。留空在入口脚本旁导出本手册。新题目包含 main.c、按模块划分的 ctb_*.h、prepare.py 和本手册；旧目录不覆盖，已有题目会提示运行其中的 prepare.py。每题保留自己的版本，更新入口脚本不改变旧题。

~~~sh
python prepare.py           # 单个 C 文件直接处理；多个先选；展开、编译检查、复制
python prepare.py --run     # 编译运行，保留标准输入输出
python prepare.py --no-copy # 只检查和保存
python prepare.py --cc "编译器路径"
~~~

提交源码另存 .toolbox/submit.c，不覆盖原文件，也不参与下次扫描。无编译器或编译失败不复制；剪贴板不可用时从该文件复制。手动编译：

~~~sh
cc -std=c99 -O2 main.c -o main -lm
~~~

本地双引号 include 自动展开；系统尖括号 include 保留。支持相对子目录和有保护宏的重复引用；不支持宏形式 include 或本地循环引用。条件分支中的本地引用也必须存在。只做编译检查，不保证题目答案正确。

## 输入输出

基础 IO：`#include "ctb_io.h"`。普通 read 返回值，失败退出；try_read 返回含 status、value 的结构体，使用 `r.status == IO_OK` 判断成功，不能当布尔值。

| 任务 | 接口 |
| --- | --- |
| 标量 | read_int / read_ll / read_uint / read_ull / read_double |
| 检查读取 | try_read_int() 等，返回值失败时为零，但仅成功时有效 |
| 字符 | read_char() 跳过空白；read_char_raw() 保留空白 |
| 输出 | print_int(x)、print_text("YES\\n")、print_newline() |
| 小数 | print_double(x)、print_fixed(x, digits) |
| 输入控制 | io_skip_line()、io_skip_whitespace() |
| 文件 | IOReader r = io_reader(file); io_try_read_int(&r) 等 |

字符串读写随 `ctb_string.h`：read_word()、read_line()、read_line_raw()、read_rest()；检查版返回 StringReadResult。line 跳过空白行并修剪两端；raw 保留空行和两端空白，移除行终止符。数值之后 raw 从当前分隔符开始，明确丢弃行尾用 io_skip_line()。返回的 String 由调用者 string_free。

数值向量分别引入 `ctb_vec_int.h`、`ctb_vec_ll.h`、`ctb_vec_double.h`，提供 read_ints/read_lls/read_doubles(count) 和 print_ints/print_lls/print_doubles(&v, separator, end)。每个头文件只定义对应类型。返回向量由调用者释放。


## String 与字符串列表

~~~c
String text = read_line();
StringList words = string_split_whitespace(&text);
string_list_sort(&words);
string_list_unique_sorted(&words);
print_strings(&words, " ", "\n");
string_list_free(&words);
string_free(&text);
~~~

| 操作 | 接口 |
| --- | --- |
| 创建 / 复制 / 长度 / 判空 | string_new() / string_from("abc") / string_clone(&s) / string_size(&s) / string_empty(&s) |
| 赋值 / 追加 / 插入 | string_assign(&s,"a") / string_append(&s,"b") / string_insert(&s,pos,"x") |
| String 直接作参数 | 以上加 _s：string_append_s(&s,&other)；匹配、替换、分割也有 _s 版本 |
| 删区间 / 切片 | string_erase(&s,pos,count) / string_slice(&s,pos,count) |
| 查找 / 包含 / 判等 | string_find(&s,"ab",start) / string_contains(&s,"ab") / string_equal(&s,&other) |
| 与字面量判等 / 按字典序比较 | string_equal_text(&s,"abc") / string_compare(&s,&other) |
| 替换 / 删除匹配 | string_replace_all(&s,old,new) / remove_first / remove_all / remove_repeated |
| 子串计数 | string_count(&s,key) 不重叠；string_count_overlapping(&s,key) 重叠 |
| 去两端空白 / 大小写 / 反转 / 回文 | string_trim / lower / upper / reverse / is_palindrome，均传 &s |
| 读写指定字节 | string_at(&s,i) / string_set(&s,i,ch) |
| 分割 / 连接 | string_split(&s,",") / string_split_whitespace(&s) / string_join(&words,",") |
| 格式化 | string_format("%04d",n) 返回 String；string_append_format(&s," %.2f",x) |
| 数值转换 | string_from_int/ll/double；string_to_int/ll/double(&s)，后者需 io 模块 |

find 未找到返回 SIZE_MAX。字面分割保留空项，空白分词不保留。删除 "aabb" 中的 "ab"：remove_all 得到 "ab"，remove_repeated 反复匹配后得到空串。全部按字节处理；反转不适合直接处理多字节中文。格式串和参数类型必须匹配。

| StringList 操作 | 接口 |
| --- | --- |
| 创建 / 长度 / 取项 | string_list_new() / size(&list) / get(&list,i)，函数名前均加 string_list_ |
| 追加 | push(&list,&s) 深复制；push_text(&list,"x")；push_take(&list,&s) 转移并清空 s |
| 修改 / 删除 / 弹出 | set / insert / erase(&list,pos,count) / pop(&list,&out) |
| 复制 / 切片 / 查找 | clone / slice(&list,pos,count) / find(&list,&s) / contains |
| 排序 / 去重 / 清空 / 释放 | sort / unique_sorted / clear / free |

列表 get 返回借用的 const String*；at 返回可修改 String*，不要单独释放它。默认复制与切片为深复制，列表 free 自动逐项释放。pop 转移给已初始化且位于列表之外的 String，或传 NULL 直接丢弃。扩容、排序、删除后不要继续使用旧的元素指针。

## Vector

默认提供 IntVector、LongLongVector、DoubleVector。以下以 IntVector 为例：

| 操作 | 调用 |
| --- | --- |
| 创建 / 字面值 / 连续整数 | IntVector_new() / IntVector_values(1,2,3) / IntVector_range(start,stop,step) |
| 追加 / 弹出 / 插入 / 删除 | _push(&v,x) / _pop(&v,&x) / _insert(&v,pos,x) / _erase(&v,pos,count) |
| 长度 / 判空 / 访问 / 修改 | _size(&v) / _empty(&v) / _at(&v,i) / _set(&v,i,x) |
| 复制 / 切片 / 拼接 | _clone(&v) / _slice(&v,pos,count) / _extend_vector(&v,&other) |
| 填充 / 改长度 / 反转 | _fill(&v,x) / _resize(&v,n,fill) / _reverse(&v) |
| 排序 / 去重 | _sort_asc(&v) / _sort_desc(&v) / _unique_sorted(&v) |
| 查找 / 包含 / 次数 | _find(&v,x) / _contains(&v,x) / _count(&v,x) |
| 二分边界 | _lower_bound(&v,x) 首个 >=x；_upper_bound 首个 >x，要求升序 |
| 最值 / 首个最值下标 | _min / _max / _argmin / _argmax |
| 统计 | _sum / _mean / _variance / _stddev / _median，均传 &v |
| 清空 / 释放 | _clear(&v) 保留容量；_free(&v) 释放并清零 |

values 至少一个参数；空容器用 new。range 不包含 stop，step 可负，不能为 0；提供于 int 和 long long 容器。slice 是起点加数量，不是 Python 的起点/终点，不支持负下标或步长。切片复制，不与原容器共享存储。

find 未找到返回 SIZE_MAX；二分边界可能返回 size，访问前检查。at 越界报错；get(&v,i,&x) 则返回 bool。sum 空容器为 0，其余统计及最值要求非空。variance 为总体方差（除以 n），median 不改原顺序。整数 sum 每步检查 long long 溢出；浮点操作要求有限数，判等为精确相等。

map / filter / reduce：普通 C 回调，无 lambda；结果立即生成，同输入元素类型。返回新容器后需释放：

~~~c
static bool positive(int x) { return x > 0; }
static int square(int x) { return x * x; }
static int add(int a, int b) { return a + b; }
/* 在 main 内： */
IntVector a = IntVector_values(-2, 1, 3);
IntVector b = IntVector_filter(&a, positive);
IntVector c = IntVector_map(&b, square);
print_int(IntVector_reduce(&c, 0, add), "\n"); /* 10 */
IntVector_free(&c); IntVector_free(&b); IntVector_free(&a);
~~~

还有 _any / _all / _count_if / _find_if、_map_inplace。空容器 any 为 false、all 为 true、reduce 返回初值。回调中不修改输入容器长度；回调自身的溢出与内部资源由调用者负责。通用 Vector 元素浅复制；字符串序列优先用 StringList。

## Map、Set、Counter

当前键为字符串，Map 值为 long long。原接口接字面量，_s 接 String。

| 类型 | 常用操作 |
| --- | --- |
| Map | map_new；set/get/get_or/contains/remove；size/clear/clone/free；map_keys 返回独立 StringList |
| Set | set_new；add/contains/remove；union/intersection/difference 返回新集合；is_subset；set_from_strings(&words) |
| Counter | counter_new；counter_add_s(&c,&word,delta)；get_s；counter_from_strings(&words)；counter_keys |

~~~c
Counter counts = counter_from_strings(&words);
StringList keys = counter_keys(&counts);
string_list_sort(&keys);
for (size_t i = 0; i < string_list_size(&keys); ++i) {
    const String *key = string_list_get(&keys, i);
    print_string(key, ": ");
    print_ll(counter_get_s(&counts, key), "\n");
}
string_list_free(&keys); counter_free(&counts);
~~~

map_get 返回 bool，区分缺失和值为 0；set/add 的 false 表示原键已存在，不是分配失败。Counter 缺失项为 0，允许负计数；add 溢出返回 false 且不修改。counter_most_common_string(&c,&key,&count) 复制最高频键到已初始化 String；并列取字典序最小项。映射遍历顺序不保证，排序输出请先取 keys。

## 双端队列与堆

| 类型 | 常用操作 |
| --- | --- |
| IntDeque | _new；_push_front/back；_pop_front/back(&q,&x) 返回 bool；_front/back；_get；_size/_empty；_clear/_clone/_free |
| IntHeap | _new(compare)；_push；_peek/_pop(&h,&x) 返回 bool；_from(items,n,compare) 批量建堆；_size/_empty；_clear/_clone/_free |

队列用 push_back/pop_front，栈用 push_back/pop_back。堆的比较器与 qsort 同形，返回负数表示第一个元素优先；相等优先级不保证稳定。

~~~c
static int cmp_int(const void *a, const void *b) {
    int x = *(const int *)a, y = *(const int *)b;
    return (x > y) - (x < y); /* 最小堆；反转比较得到最大堆 */
}
~~~

## 只记住这些规则

- 对象先初始化；不要直接写未初始化的 String s。String/列表/容器使用 new 或 {0}，Heap 要先设置比较器。
- 拥有内存的对象用 clone 复制、free 释放；普通结构体赋值不是深复制。StringList 管理内部字符串，通用 Vector 不管理元素内部资源。
- 下标从 0 开始，切片与删除用起点加数量。没有特殊需求不访问 .data；原生指针仍可作兼容出口。
- 库的分配失败、非法必需参数等会报错退出；需要处理 EOF/非法输入时用 try_read_* 并显式比较状态。

工程与脚本 MIT；模板及生成 C 代码、示例 MIT-0；手册正文 CC BY 4.0。
作者 Annouimo · https://github.com/Annouimo/c-exam-kit 。生成的 C 答案可删除署名与注释。

## 位运算与基础数学（模块化接口）

| 头文件 | 接口与约定 |
| --- | --- |
| ctb_bits.h | bit_get/set/clear/toggle(value, bit)，最低位为 0，返回结果，不原地修改；bit_count(value) |
| ctb_math.h | gcd(a,b)、lcm(a,b)，输入 long long，结果 unsigned long long；负数取绝对值，gcd(0,0)=0 |
| ctb_math.h | factorial(n) 返回 unsigned long long；integer_power(base, exponent) 返回 long long，指数非负，0^0=1 |
| ctb_math.h | try_factorial / try_lcm / try_integer_power 返回 ok 与 value；失败值仅作占位 |
| ctb_math.h | round_digits(x, digits)，中点远离零，负 digits 表示向十、百位舍入；浮点表示会影响中点 |
| ctb_math.h | nearly_equal(a,b,abs_tol,rel_tol)、is_prime(n)、polynomial_value(coefficients,count,x)，系数按次数升序 |

ctb_vec_*.h 的 get/front/back/pop 直接返回元素，非法访问或空容器退出；try_get、try_pop 返回 ok/value。旧输出指针版本暂以 _into 后缀保留。sort_by 使用对应元素类型的比较器，稳定排序；sort 保留底层 qsort 比较器接口。

## 数组、前缀和、矩阵与日期

| 头文件 | 常用接口 |
| --- | --- |
| ctb_array.h | IntVector_longest_equal_run 返回 begin/end；IntVector_max_subarray 返回 ok/sum/begin/end，空输入 ok=false；均采用半开区间，最长相等段同长时取最早 |
| ctb_array.h | IntVector_difference 返回 LongLongVector，避免 int 差溢出；IntVector_next_permutation 原地推进，末尾重置升序并返回 false |
| ctb_prefix_sum.h | prefix_sum_from(&v) 创建快照；prefix_sum_query(&p,begin,end) 查询；prefix_sum_free(&p) 释放；仅支持 IntVector，溢出退出 |
| ctb_matrix.h | Matrix 为 double 元素；matrix_new(rows,cols)、get/set、clone、transpose、multiply；新对象由调用者 matrix_free |
| ctb_matrix.h | read_matrix(rows,cols)；print_matrix(&m,digits,separator)，每行换行 |
| ctb_date.h | date_new(y,m,d)、try_date、date_valid、date_compare、date_difference(a,b) 返回从 a 到 b 的天数 |
| ctb_date.h | date_add_days(d,delta)、try_date_add_days；公历 1～9999 年，无时区；read_date 读取三个空白分隔整数，print_date 输出 YYYY-MM-DD |
| ctb_date.h | TimeOfDay、time_from_seconds、time_to_seconds，时刻范围 00:00:00～23:59:59 |

拥有内存的对象不可通过普通赋值当作深拷贝。PrefixSum 释放后不可查询。矩阵不承诺整数运算精确性；需要精确大整数矩阵的情形不在当前接口范围内。
