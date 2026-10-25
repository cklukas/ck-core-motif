#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/VaSimple.c").read_text()
assert "XtTypedArgList avlist,\n        int capacity" in source
helper_start = source.index("_XmNestedArgtoTypedArg(", source.index("_XmNestedArgtoTypedArg(") + 1)
helper = source[helper_start:source.index("\n}\n", helper_start) + 2]
assert "if (args == NULL || avlist == NULL || capacity < 0)" in helper
assert "if (count >= capacity)" in helper
assert "capacity - count" in helper

converter = source[source.index("_XmVaToTypedArgList("):source.index("\n}\n", source.index("_XmVaToTypedArgList(")) + 2]
assert "if (count >= max_count)" in converter
assert "max_count - count" in converter
assert "*args_return = NULL;" in converter

print("Nested typed varargs conversion stays within its allocated capacity")
