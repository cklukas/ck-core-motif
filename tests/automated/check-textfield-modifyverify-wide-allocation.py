#!/usr/bin/env python3
"""Check wide callback insertion allocation bounds in TextField."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text()
start = source.index("/* copy the newblock.ptr, length, start, and end")
end = source.index("  if (do_free) XtFree(newblock.ptr);", start)
body = source[start:end]

assert body.count("(size_t)*insert_length > SIZE_MAX / sizeof(wchar_t) - 1") == 2
assert body.count("(size_t)*insert_length + 1 > UINT_MAX / sizeof(wchar_t)") == 2
assert body.count("goto modify_verify_allocation_failed;") >= 4
assert "(unsigned)(*insert_length + 1) * sizeof(wchar_t)" not in body

print("PASS TextField modify-verify wide allocation guards")
