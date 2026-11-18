# SPDX-License-Identifier: MIT
"""Build and use a conservative index of the toolbox's own static functions."""
import re
from pathlib import PurePosixPath
import posixpath

TOKEN = re.compile(r'''/\*[\s\S]*?\*/|//[^\n]*|"(?:\\[\s\S]|[^"\\])*"|'(?:\\[\s\S]|[^'\\])*'|[A-Za-z_]\w*''')
FUNCTION = re.compile(r"(?m)^static[ \t]+[\w \t*]+?\b([A-Za-z_]\w*)\s*\([^;{}]*\)\s*\{")
DIRECTIVE = re.compile(r"(?m)^[ \t]*#(?:[^\n\\]|\\[^\n]|\\\n)*")


def identifiers(code):
    code = code.replace("\\\n", "")
    return {match[0] for match in TOKEN.finditer(code) if match[0][0].isalpha() or match[0][0] == "_"}


def index_header(headers):
    seen = set()

    def flatten(name):
        if name in seen:
            return ""
        seen.add(name)

        def include(match):
            return flatten(posixpath.normpath(str(PurePosixPath(name).parent / match[1])))

        return re.sub(r'(?m)^#include "([^"]+)"[ \t]*\n', include, headers[name])

    full = "#ifndef CTB_SINGLE_HEADER_H\n#define CTB_SINGLE_HEADER_H\n"
    full += "\n".join(flatten(name) for name in sorted(headers) if "/custom.h" not in name)
    full += "\n#endif\n"
    masked = TOKEN.sub(lambda m: m[0] if m[0][0].isalpha() or m[0][0] == "_"
                       else re.sub(r"[^\n]", " ", m[0]), full)
    masked = DIRECTIVE.sub(lambda m: re.sub(r"[^\n]", " ", m[0]), masked)
    spans = []
    offset = 0
    while match := FUNCTION.search(masked, offset):
        depth = 1
        end = match.end()
        for brace in re.finditer(r"[{}]", masked[end:]):
            depth += 1 if brace[0] == "{" else -1
            if not depth:
                end += brace.end()
                break
        else:
            raise ValueError("Unclosed toolbox function: " + match[1])
        spans.append((match.start(), end, match[1]))
        offset = end
    for directive in DIRECTIVE.finditer(full):
        name = re.match(r"#define ([A-Za-z_]\w*_values)\(", directive[0].lstrip())
        if name:
            spans.append((directive.start(), directive.end(), name[1]))
    pieces = []
    offset = 0
    for start, end, name in sorted(spans):
        pieces.append(("", full[offset:start]))
        pieces.append((name, full[start:end]))
        offset = end
    pieces.append(("", full[offset:]))
    known = {name for name, _ in pieces if name}
    dependencies = {}
    roots = set()
    for name, code in pieces:
        refs = identifiers(code) & known
        if name:
            dependencies.setdefault(name, set()).update(refs - {name})
        else:
            roots.update(refs)
    return full, (pieces, {name: sorted(refs) for name, refs in dependencies.items()}, sorted(roots))


def assemble(source, bundle):
    normalized = source.replace("\\\n", "")
    visible = TOKEN.sub(lambda m: re.sub(r"[^\n]", " ", m[0]) if m[0].startswith(("//", "/*"))
                        else m[0], normalized)
    includes = re.finditer(r"(?m)^[ \t\v\f]*(?:#|%:|\?\?=)[ \t]*([^\n]*)", visible)
    toolbox = []
    for match in includes:
        directive = match[1].strip()
        if re.fullmatch(r'include\s+"c_toolbox\.h"', directive):
            toolbox.append(visible.count("\n", 0, match.start()))
        elif not re.fullmatch(r"include\s+<[^>]+>", directive):
            raise ValueError("单头文件模式仅支持 c_toolbox.h 和标准库 #include。")
    if len(toolbox) != 1 or normalized != source:
        raise ValueError("请使用一行 #include \"c_toolbox.h\"，不要使用续行。")
    pieces, dependencies, roots = bundle
    needed = set()
    pending = list((identifiers(source) & dependencies.keys()) | set(roots))
    while pending:
        name = pending.pop()
        if name not in needed:
            needed.add(name)
            pending.extend(dependencies[name])
    selected = "".join(code for name, code in pieces if not name or name in needed)
    lines = re.findall(r"[^\n]*\n|[^\n]+$", source)
    i = toolbox[0]
    if not re.fullmatch(r'[ \t]*#[ \t]*include[ \t]+"c_toolbox\.h"[ \t]*\n?', lines[i]):
        raise ValueError("请将 c_toolbox.h 的引入单独放在一行。")
    selected = re.sub(r"(?m)(?<!\\\n)^[ \t]*\n", "", selected)
    return '#line 1 "main.c"\n' + "".join(lines[:i]) + selected + '\n#line %d "main.c"\n' % (i + 2) + "".join(lines[i + 1:])
