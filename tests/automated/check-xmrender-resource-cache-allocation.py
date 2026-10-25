#!/usr/bin/env python3
"""Check XmRenderTable resource-cache allocations fail safely."""
from pathlib import Path

source = Path("lib/Xm/XmRenderT.c").read_text()
start = source.index("static Boolean\nGetResources(")
end = source.index("\n/* Sets all resources", start)
region = source[start:end]
assert "if (quarks == NULL)" in region
assert "if (found == NULL)" in region
assert "SIZE_MAX / sizeof(XrmQuark)" in region
assert "UINT_MAX / sizeof(XrmQuark)" in region
assert "XrmQuark *new_quarks" in region
assert "if (new_quarks == NULL)" in region
assert region.index("new_quarks =") < region.index("quarks = new_quarks")
assert "next_size = searchListSize * 2" in region
assert "SIZE_MAX /" in region
assert "UINT_MAX /" in region
assert "new_search_list == NULL" in region
assert "goto resource_failure" in region
assert "SIZE_MAX / sizeof(XrmResource)" in source
assert "UINT_MAX / sizeof(XrmResource)" in source
assert "if (table == NULL)" in region
print("XmRenderTable resource-cache allocations are guarded")
