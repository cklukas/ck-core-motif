#!/usr/bin/env python3
"""Check RowColumn accelerator text allocation failure handling."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "RowColumn.c").read_text()
start = source.index("static Boolean\nAppendRealKeyText(")
end = source.index("\nstatic char * \nGetRealKey", start)
append_region = source[start:end]
assert "String new_buffer;" in append_region
assert "if (next == 0)" in append_region
assert "new_buffer = (String)XtRealloc(*buffer, next);" in append_region
assert "if (new_buffer == NULL)" in append_region
assert "return False;" in append_region

start = source.index("static char * \nGetRealKey(")
end = source.index("\nstatic void \nMenuBarInitialize", start)
key_region = source[start:end]
assert "buf = (String)XtMalloc(capacity);" in key_region
assert "if (buf == NULL)" in key_region
assert key_region.count("buf = (String)XtMalloc(capacity);") == 1
print("PASS RowColumn accelerator text checks allocation failures")
