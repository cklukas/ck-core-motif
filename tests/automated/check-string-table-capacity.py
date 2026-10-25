#!/usr/bin/env python3
"""Check that String-table growth fits Xt's Cardinal allocation size."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ResConvert.c").read_text()
start = source.index("CvtStringToStringTable(", 10000)
end = source.index("\n/*ARGSUSED*/\nstatic Boolean", start)
region = source[start:end]
assert "UINT_MAX / sizeof(String)" in region
assert "UINT_MAX / (2 * sizeof(String))" in region
assert region.count("(Cardinal)(sizeof(String) *") >= 3
assert "if ((unsigned int)i >= UINT_MAX / sizeof(String))" in region
print("String-table growth checks Xt allocation capacity")
