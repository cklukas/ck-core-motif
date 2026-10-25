#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/XmRenderT.c").read_text()
start = source.index("#define CVTaddString")
end = source.index("/*ARGSUSED*/", start)
body = source[start:end]
assert "char *new_buffer" in body
assert "new_size > UINT_MAX" in body
assert "new_buffer = (char *)XtRealloc" in body
assert "if (new_buffer == NULL)" in body
assert "buffer = new_buffer;" in body
print("XmRenderTable property-buffer growth guards present")
