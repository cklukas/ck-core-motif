#!/usr/bin/env python3
"""Check Text source buffers are bounded before Xt Cardinal allocation."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextStrSo.c").read_text()
start = source.index("static XmTextPosition")
start = source.index("ReadSource(", start)
end = source.index("_XmTextValidate(", start)
body = source[start:end]

assert "block_bytes > (size_t)UINT_MAX" in body
assert "XtMalloc((Cardinal)block_bytes)" in body
assert "XtRealloc(data->value, (Cardinal)block_bytes)" in body

print("Text source allocations are Cardinal-bounded")
