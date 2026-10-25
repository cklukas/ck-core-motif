#!/usr/bin/env python3
"""Check that file-selector filter construction handles allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xmos.c").read_text(encoding="utf-8")
start = source.index("_XmOSQualifyFileSpec(")
end = source.index("GetFixedMatchPattern(String pattern)", start)
region = source[start:end]
assert "fSpec = XtMalloc(filterLen + 2);" in region
assert "if (fSpec == NULL)" in region
assert "dSpec = XtMalloc(filterLen + dirLen + 4);" in region
assert "if (dSpec == NULL)" in region
assert "XtFree(fSpec);" in region
print("PASS fixed-match filter allocations are guarded")
