#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/AccessibleDBus.c").read_text()
start = source.index("#define ADD_BREAK")
end = source.index("    ADD_BREAK(0);", start)
body = source[start:end]
assert "int *new_breaks" in body
assert "if (!new_breaks) goto range_failed;" in body
assert "breaks = new_breaks;" in body
assert "capacity = next;" in body
print("AccessibleDBus break-array growth guards present")
