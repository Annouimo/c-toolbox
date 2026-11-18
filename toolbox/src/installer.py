# SPDX-License-Identifier: MIT
import hashlib
import os
import shlex
import subprocess

def guarded_header(code):
    guard = "C_TOOLBOX_" + hashlib.sha256(code.encode("utf-8")).hexdigest()[:20].upper() + "_H"
    return "#ifndef " + guard + "\n#define " + guard + "\n\n" + code + "\n#endif\n"

def all_header():
    modules = [name for name in BUNDLE["catalog"]["modules"] if name != "core"]
    return guarded_header(generate(modules))

SKELETONS = {
    "blank": "",
    "t": "int t = read_int();\n    if (t < 0) tb_fail(\"negative test count\");\n    for (int i = 0; i < t; ++i) {\n        /* TODO */\n    }",
    "eof": "for (;;) {\n        IntReadResult r = try_read_int();\n        if (r.status == IO_EOF) break;\n        io_require(r.status);\n        int value = r.value;\n        (void)value; /* TODO */\n    }",
    "sentinel": "for (;;) {\n        IntReadResult r = try_read_int();\n        if (r.status == IO_EOF) break;\n        io_require(r.status);\n        if (r.value == -1) break; /* change sentinel */\n        /* TODO */\n    }",
    "operations": "int q = read_int();\n    if (q < 0) tb_fail(\"negative operation count\");\n    for (int i = 0; i < q; ++i) {\n        char operation = read_char();\n        switch (operation) {\n            /* TODO: case */\n        }\n    }",
}

SKELETON_MENU = (
    ("blank", "1  空白 main（默认）"),
    ("t", "2  T 组数据"),
    ("eof", "3  读到 EOF"),
    ("sentinel", "4  遇哨兵停止"),
    ("operations", "5  q 次操作"),
    ("toolbox_only", "6  仅创建工具箱，不生成 main"),
)

def starter(skeleton):
    if skeleton not in SKELETONS:
        raise ValueError("unknown input skeleton: " + skeleton)
    return '#include "ctb/io.h"\n\nint main(void) {\n    ' + SKELETONS[skeleton] + '\n    return 0;\n}\n'

def initialize_folder(answer, base=None, skeleton="blank", single_header=True):
    base = Path(base or Path(__file__).resolve().parent).resolve()
    version = BUNDLE["catalog"]["version"]
    if not answer.strip():
        target = base / ("工具箱速查_v" + version + ".md")
        text = BUNDLE["manual"]
        if target.exists():
            if target.read_text(encoding="utf-8") != text:
                raise ValueError("手册已存在且内容不同，不覆盖：" + str(target))
        else:
            with target.open("x", encoding="utf-8") as stream:
                stream.write(text)
        print("已导出手册：" + str(target))
        return target
    requested = Path(answer.strip()).expanduser()
    target = (requested if requested.is_absolute() else base / requested).resolve()
    if target.exists():
        message = "文件夹已存在，为保护已有题目未写入任何文件：" + str(target)
        helper = target / "prepare.py"
        if helper.is_file():
            command = [sys.executable, str(helper)]
            display = subprocess.list2cmdline(command) if os.name == "nt" else shlex.join(command)
            message += "\n准备提交请运行：\n" + display
        raise ValueError(message)
    if skeleton not in SKELETONS and skeleton != "toolbox_only":
        raise ValueError("unknown project skeleton: " + skeleton)
    contents = ({"c_toolbox.h": BUNDLE["single_header"], "prepare.py": BUNDLE["single_prepare"]}
                if single_header else dict(BUNDLE["headers"], **{"prepare.py": BUNDLE["prepare"]}))
    if skeleton != "toolbox_only":
        contents["main.c"] = starter(skeleton)
        if single_header:
            contents["main.c"] = contents["main.c"].replace("ctb/io.h", "c_toolbox.h", 1)
    target.parent.mkdir(parents=True, exist_ok=True)
    target.mkdir(exist_ok=False)
    created = []
    try:
        for name, content in contents.items():
            path = target / name
            path.parent.mkdir(parents=True, exist_ok=True)
            with path.open("x", encoding="utf-8") as stream:
                created.append(path)
                stream.write(content)
    except Exception:
        for path in created:
            path.unlink()
        for directory in sorted((p for p in target.rglob("*") if p.is_dir()),
                                key=lambda p: len(p.parts), reverse=True):
            directory.rmdir()
        target.rmdir()
        raise
    print("已创建题目目录：" + str(target))
    if skeleton == "toolbox_only":
        print("工具箱已创建；添加 C 源文件后，运行同目录 prepare.py。")
    else:
        print("编辑 main.c；提交时运行同目录 prepare.py。")
    print("旧题的头文件和 prepare.py 保持原版本，不随入口脚本更新。")
    return target

def cli(argv=None):
    for stream in (sys.stdin, sys.stdout, sys.stderr):
        if hasattr(stream, "reconfigure"):
            stream.reconfigure(encoding="utf-8")
    args = list(sys.argv[1:] if argv is None else argv)
    if args and (args[0] in ("generate", "header") or
                 any(x in args for x in ("--modules", "--list", "--vector", "--deque",
                                         "--heap", "--preamble", "--main", "--header", "--version"))):
        if args[0] == "generate":
            return generate_cli(args[1:])
        if args[0] == "header":
            return generate_cli(["--header"] + args[1:])
        return generate_cli(args)
    parser = argparse.ArgumentParser(description="C Toolbox: create a problem folder; Enter exports Markdown.")
    parser.add_argument("--folder", help="noninteractive initialization; empty value exports the manual")
    parser.add_argument("--skeleton", choices=[key for key, _ in SKELETON_MENU], default=None)
    mode = parser.add_mutually_exclusive_group()
    mode.add_argument("--modular", dest="single_header", action="store_false",
                      help="生成分模块头文件，提交时完整展开")
    mode.add_argument("--single-header", dest="single_header", action="store_true",
                      help="生成单头文件，提交时按需组装（默认）")
    parser.set_defaults(single_header=True)
    options = parser.parse_args(args)
    try:
        if options.folder is None:
            print("C Toolbox · 创建题目")
            answer = input("题目目录（回车导出手册）：")
        else:
            answer = options.folder
        choice = options.skeleton or "blank"
        if answer.strip() and options.folder is None and options.skeleton is None:
            requested = Path(answer.strip()).expanduser()
            target = requested if requested.is_absolute() else Path(__file__).resolve().parent / requested
            if not target.exists():
                print("\n选择程序框架")
                for _, label in SKELETON_MENU:
                    print(label)
                selection = input("选择编号，直接生成项目（回车默认 1）：").strip() or "1"
                choices = {str(index): key for index, (key, _) in enumerate(SKELETON_MENU, start=1)}
                if selection not in choices:
                    raise ValueError("请选择 1 到 6。")
                choice = choices[selection]
        initialize_folder(answer, skeleton=choice, single_header=options.single_header)
    except (ValueError, OSError) as error:
        print(str(error), file=sys.stderr)
        return 1
    except (KeyboardInterrupt, EOFError):
        print("已取消。", file=sys.stderr)
        return 130
    return 0

if __name__ == "__main__":
    raise SystemExit(cli())
