#!/usr/bin/env python3
"""Check input-method varargs argument allocation failure handling."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmIm.c").read_text()
start = source.rindex("ImCreateArgList(va_list")
end = source.index("\n}\n", start) + 3
allocator = source[start:end]
assert "if (total_count <= 0 ||" in allocator
assert "(size_t)total_count > SIZE_MAX / sizeof(Arg)" in allocator
assert "(size_t)total_count > UINT_MAX / sizeof(Arg)" in allocator
assert "if (args == NULL)" in allocator
assert allocator.index("if (args == NULL)") < allocator.index("args[i].name")
counter_start = source.rindex("ImCountVaList(va_list")
counter_end = source.index("\n}\n", counter_start) + 3
counter = source[counter_start:counter_end]
assert "if (total_count == NULL)" in counter
assert "if (*total_count == INT_MAX)" in counter
assert "return False;" in counter
for marker in ("XmImVaSetFocusValues", "XmImVaSetValues"):
    start = source.index(marker)
    end = source.index("\n}\n", start) + 3
    region = source[start:end]
    assert "if (!ImCountVaList(var, &total_count))" in region
    assert "total_count != 0 && args == NULL" in region
    assert "(Cardinal)total_count" in region
print("PASS XmIm varargs allocation failure handling")
