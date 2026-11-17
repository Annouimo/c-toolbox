# 旧组件处理结果

记录新公共接口的去向；删除项遵循已确认的取舍。数组算法当前主要覆盖 IntVector；矩阵现在覆盖 int、long long 与 double，整数乘法检查溢出。

| 序号 | 旧组件 | 新去向 |
| --- | --- | --- |
| 1 | 进制与数位 | ctb/math/number.h |
| 2 | GCD/LCM | ctb/math/arithmetic.h |
| 3 | 试除判素数 | ctb/math/arithmetic.h |
| 4 | 素数筛 | ctb/math/sieve.h |
| 5 | 二进制显示 | 合并到 integer_to_base |
| 6 | 位操作 | ctb/math/bits.h |
| 7 | 普通阶乘 | ctb/math/arithmetic.h |
| 8 | 整数幂与浮点比较 | ctb/math/arithmetic.h |
| 9 | 四舍五入 | ctb/math/arithmetic.h |
| 10 | 固定小数与负零 | ctb/io.h：print_fixed |
| 11 | 多项式 | ctb/math/arithmetic.h |
| 12 | 二次方程 | 删除 |
| 13 | 二分求根 | ctb/search.h |
| 14 | 复数 | ctb/math/complex.h，删除计算器流程 |
| 15 | 高精度加法 | ctb/math/bigint.h |
| 16 | 高精度减法 | ctb/math/bigint.h |
| 17 | 高精度乘法 | ctb/math/bigint.h |
| 18 | 高精度除法 | ctb/math/bigint.h |
| 19 | 高精度小整数除法 | ctb/math/bigint.h |
| 20 | 高精度阶乘 | ctb/math/bigint.h |
| 21 | 数组统计 | ctb/vector/int.h、ctb/vector/long_long.h、ctb/vector/double.h |
| 22 | 频次 | 公开模块已移除 |
| 23 | 最长连续段 | ctb/sequence/algorithms.h：保留相等段，删除逐项加一 |
| 24 | 最大子段 | ctb/sequence/algorithms.h：IntVector |
| 25 | 前向差分 | ctb/sequence/algorithms.h：IntVector → LongLongVector |
| 26 | 前缀和 | ctb/sequence/prefix_sum.h：IntVector 快照 |
| 27 | RMQ/ST | 删除 |
| 28 | 矩阵乘法 | ctb/matrix/double.h、ctb/matrix/int.h、ctb/matrix/long_long.h |
| 29 | 数值排序 | ctb/vector/int.h、ctb/vector/long_long.h、ctb/vector/double.h |
| 30 | 排序查找 | 数值 Vector：binary_search/find |
| 31 | 结构体排序 | ctb/vector/custom.h：自定义 sort_by |
| 32 | 原位置 | ctb/sequence/algorithms.h：IntVector_argsort |
| 33 | 二分边界 | 数值 Vector：lower_bound/upper_bound |
| 34 | 排名去重 | 去重与离散化保留；特定排名规则删除 |
| 35 | 答案二分 | ctb/search.h |
| 36 | 字符串基础 | ctb/string.h |
| 37 | 字符串排序 | ctb/string.h：StringList |
| 38 | 最长连续字符 | ctb/string.h：string_longest_char_run |
| 39 | 日期 | ctb/date.h |
| 40 | 日期推移 | ctb/date.h：天数推移 |
| 41 | 时间 | ctb/date.h：TimeOfDay |
| 42 | 排列 | ctb/sequence/algorithms.h：IntVector_next_permutation |
| 43 | 组合 | ctb/sequence/combination.h |
| 44 | 记忆化递归 | 删除 |
| 45 | 栈与括号匹配 | Vector 保留栈操作；删除括号匹配 |
| 46 | 网格 DFS | 删除 |
| 47 | 网格路径 | 删除 |

输入骨架已改用新 IO，在创建目录时选择。旧示例仅作测试夹具，用户用法集中在 README 与速查手册。跨平台验证以当前提交 CI 为准。
