#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/IconG.c").read_text()
assert "#include <stdint.h>" in source
assert "Cardinal new_max = MAX(count, 33);" in source
assert "SIZE_MAX / sizeof(XmString)" in source
assert "UINT_MAX / sizeof(XmString)" in source
assert "XmStringTable new_table;" in source
assert "if (new_table == NULL)" in source
assert "order[i] > 0 && order[i] <= st_count" in source
print("IconG reordered-table guards present")
