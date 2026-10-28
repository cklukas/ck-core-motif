#!/usr/bin/env python3
"""Check TextField warning formatting validates its allocation size."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text()
start = source.index("err_str = NULL;")
end = source.index("if (err_str != NULL)", start)
body = source[start:end]

assert "SIZE_MAX - 1" in body
assert "UINT_MAX / 4" in body
assert "warning_bytes = (size_t)num_conv * 4 + 1" in body
assert "XtMalloc((Cardinal)warning_bytes)" in body
assert "XtMalloc((4 * num_conv) + 1)" not in body
print("TextField warning allocation is checked")
