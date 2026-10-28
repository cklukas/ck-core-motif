#!/usr/bin/env python3
"""Check clipboard assembly preserves the buffer on XtRealloc failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("static int \nGetWindowProperty")
end = source.index("static Boolean\nClipboardHeaderRecordValid", start)
body = source[start:end]

assert "char *new_pointer;" in body
assert "new_pointer = XtRealloc(cur_pointer" in body
assert "if (new_pointer == NULL)" in body
assert "cur_pointer = new_pointer;" in body
assert "if (cur_pointer != NULL)\n\tXtFree (cur_pointer);" in body

print("Clipboard assembly preserves Xt-owned buffers on growth failure")
