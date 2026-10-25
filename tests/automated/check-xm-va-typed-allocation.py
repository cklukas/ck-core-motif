#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/VaSimple.c").read_text()
start = source.index("_XmVaToTypedArgList(")
region = source[start:source.index("\n}\n", start) + 2]

assert "*args_return = NULL;" in region
assert "*num_args_return = 0;" in region
assert "max_count <= 0" in region
assert "(size_t)max_count >= SIZE_MAX / sizeof(XtTypedArg)" in region
assert "(unsigned int)max_count >= UINT_MAX / sizeof(XtTypedArg)" in region
assert "XtMalloc((size_t)max_count * sizeof(XtTypedArg));" in region
assert "if (args == NULL)" in region

print("_XmVaToTypedArgList validates count and reports allocation failure safely")
