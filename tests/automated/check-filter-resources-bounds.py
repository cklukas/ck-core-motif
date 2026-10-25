#!/usr/bin/env python3
"""Check that legacy resource filtering has no fixed index limit."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Obso2_0.c").read_text(
    encoding="utf-8"
)
start = source.index("_XmFilterResources(")
end = source.index("/************************************************************************", start)
region = source[start:end]

assert "copyIndexes[256]" not in region
assert "SIZE_MAX / sizeof(XtResource)" in region
assert "UINT_MAX / sizeof(XtResource)" in region
assert "XtMalloc((Cardinal)(j *" in region
assert "filteredResources == NULL && j != 0" in region
assert region.count("resource_offset >= filterOffset") == 2
assert "if (filteredResourcesRtn == NULL)" in region
assert "if (resources == NULL || filterClass == NULL)" in region
print("PASS legacy resource filtering is bounds-safe")
