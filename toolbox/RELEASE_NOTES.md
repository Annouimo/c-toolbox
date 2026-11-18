# C Toolbox v0.9.1

- 新增布尔向量和布尔矩阵的 `0/1` 输出。
- 矩阵打印统一使用 `elem_sep, row_sep, end`。例如 `print_int_matrix(&m, " ", "\n", "\n")`。迁移旧调用时需要补上行间分隔符，字符矩阵还需指定行内分隔符。
- Vector 和 Matrix 的 `set` 统一为越界报错。原来检查 Vector `set` 返回值的调用请换成 `try_set`。
- Vector 新增 `filled`、`in_bounds`。Matrix 新增尺寸查询和 `resize`，缩放保留左上角重叠区域。
