#!/usr/bin/env python3
"""Check input-method varargs argument allocation failure handling."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmIm.c").read_text()
start = source.rindex("ImCreateArgList(va_list")
end = source.index("\n}\n", start) + 3
allocator = source[start:end]
assert "if (total_count <= 0)" in allocator
assert "if (args == NULL)" in allocator
assert allocator.index("if (args == NULL)") < allocator.index("args[i].name")
for marker in ("XmImVaSetFocusValues", "XmImVaSetValues"):
    start = source.index(marker)
    end = source.index("\n}\n", start) + 3
    region = source[start:end]
    assert "total_count != 0 && args == NULL" in region
print("PASS XmIm varargs allocation failure handling")
