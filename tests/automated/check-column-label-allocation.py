#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/Column.c").read_text()
start = source.index('buf_length = snprintf(NULL, 0, "%s_label"')
end = source.index('XiC(new_w)->label_widget = label;', start)
region = source[start:end]
assert "if (!buf)" in region
assert "(size_t)buf_length + 1 > UINT_MAX" in region
assert "XtMalloc((Cardinal)((size_t)buf_length + 1))" in region
assert "label = XtCreateWidget" in region
assert "if (!label)" in region
assert "label_widget = False;" in region
print("Column label allocation guards present")
