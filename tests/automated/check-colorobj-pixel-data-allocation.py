#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/ColorObj.c").read_text()
start = source.index("FetchPixelData(")
end = source.index("/* read color use */", start)
region = source[start:end]
assert "bounded_value = (char *)XtMalloc" in region
assert "if (bounded_value == NULL)" in region
assert "XFree(value);" in region
assert region.index("if (bounded_value == NULL)") < region.index("memcpy(bounded_value")
print("ColorObj pixel-data allocation guard present")
