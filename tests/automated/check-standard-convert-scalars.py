#!/usr/bin/env python3
"""Check scalar standard-conversion allocation failure paths."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("XmeStandardConvert(")
end = source.index("\nvoid\n_XmConvertComplete", start)
region = source[start:end]

assert region.count("if (fg == NULL)") == 1
assert region.count("if (bg == NULL)") == 1
assert region.count("if (cmap == NULL)") == 1
assert region.count("cs -> length = 0;") >= 4
assert region.count("_XmAppUnlock(app);") >= 4

print("PASS standard conversion scalar allocation checks")
