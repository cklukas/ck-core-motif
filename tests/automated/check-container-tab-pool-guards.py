#!/usr/bin/env python3
"""Check Container dynamic tab-pool allocation cleanup."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text()
start = source.rindex("GetDumbTabList(")
end = source.index("GetDynFirstColWidth(", start)
region = source[start:end]
assert "if (tab_pool == NULL)" in region
assert "UINT_MAX / sizeof(*tab_pool)" in region
assert "tab_bytes = (size_t)asked_num_tab * sizeof(*tab_pool);" in region
assert "XtMalloc((Cardinal)tab_bytes)" in region
assert "if (tab_pool[i] == NULL)" in region
assert "XmTabFree(tab_pool[i]);" in region
assert "XtFree((char *)tab_pool);" in region
print("PASS Container dynamic tab-pool allocation is transactional")
