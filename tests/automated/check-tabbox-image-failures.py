#!/usr/bin/env python3
"""Check tab-box image rotation handles allocation and image creation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TabBox.c").read_text(
    encoding="utf-8"
)
start = source.index("data = (char*) XtMalloc")
end = source.index("return( dst );", start)
region = source[start:end]
assert "if (data == NULL)" in region
assert region.count("if (dst == NULL)") == 2
assert region.count("XtFree(data)") == 2
print("tab-box image rotation handles allocation and image creation failure")
