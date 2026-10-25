#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/VaSimple.c").read_text()
first = source.index("_XmVaProcessEverything(", source.index("static Boolean"))
start = source.index("_XmVaProcessEverything(", first + 1)
region = source[start:source.index("\n}\n", start) + 2]

assert "static Boolean" in source[source.rfind("static Boolean", 0, start):start]
assert "(size_t)num_args > SIZE_MAX / sizeof(Arg)" in region
assert "(size_t)num_args > UINT_MAX / sizeof(Arg)" in region
assert "goto allocation_failure" in region
assert "return False;" in region
assert "return True;" in region
assert source.count("if (!_XmVaProcessEverything(") == 6
assert "_XmVaAddCount" in source
assert "base > INT_MAX - extra" in source
assert source.count("if (!_XmVaAddCount(args_count,") == 6

print("VaSimple menu argument processing checks allocation bounds and failures")
