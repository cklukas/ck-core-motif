#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/VaSimple.c").read_text()
start = source.index("XmeVLCreateWidget(")
region = source[start:source.index("\n}\n", start) + 2]

assert "(size_t)count >= SIZE_MAX / sizeof(Arg)" in region
assert "(unsigned int)count >= UINT_MAX / sizeof(Arg)" in region
assert "args = count ? (ArgList)XtMalloc((size_t)count * sizeof(Arg)) : NULL;" in region
assert "if (count != 0 && args == NULL)" in region
assert "_XmAppUnlock(app);" in region

print("XmeVLCreateWidget validates varargs count and allocation failure")
