#!/usr/bin/env python3
"""Check XmString baseline extraction handles invalid outputs and allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
start = source.index("_XmStringGetBaselines(")
end = source.index("void\n_XmStringGetCurrentCharset", start) if "void\n_XmStringGetCurrentCharset" in source[start:] else start + 4000
region = source[start:end]
assert "if (baselines == NULL || line_count == NULL)" in region
assert region.count("if (*baselines == NULL)") == 2
assert region.count("*line_count = 0;") >= 2
assert "UINT_MAX / sizeof(Dimension)" in region
assert region.count("XtMalloc((Cardinal)") >= 2
assert "XtMalloc(*line_count * sizeof(Dimension))" not in region
print("PASS XmString baseline extraction handles allocation failure")
