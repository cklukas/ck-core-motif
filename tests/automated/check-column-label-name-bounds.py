#!/usr/bin/env python3
"""Check that Column preserves long generated label names safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Column.c").read_text()
start = source.index("buf_length = snprintf(NULL, 0, \"%s_label\"")
end = source.index("label_widget = True;", start)
region = source[start:end]
assert 'buf_length = snprintf(NULL, 0, "%s_label"' in region
assert "buf = XtMalloc((Cardinal)((size_t)buf_length + 1))" in region
assert 'snprintf(buf, (size_t)buf_length + 1, "%s_label"' in region
assert "strncpy(buf, XtName(new_w), 240)" not in region
assert "strcat(buf, \"_label\")" not in region
assert "XtFree(buf);" in source[source.index("label = XtCreateWidget", start):]
print("Column generated label names preserve long widget names")
