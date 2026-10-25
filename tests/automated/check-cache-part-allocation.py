#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/Cache.c").read_text()
start = source.index("_XmCachePart(")
end = len(source)
region = source[start:end]
assert "SIZE_MAX - XtOffsetOf" in region
assert "allocation_size" in region
assert region.count("if (new_ptr == NULL)") == 1
assert region.count("if (ptr == NULL)") == 1
assert region.count("return NULL;") >= 3
print("Gadget cache allocation guards present")
