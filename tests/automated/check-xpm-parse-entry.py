#!/usr/bin/env python3
"""Check XPM parsing validates parser and output arguments."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xpmparse.c").read_text()
start = source.index("xpmParseData(\n")
start = source.index("{", start)
end = source.index("\n}", start)
function = source[start:end]
assert "if (data == NULL || image == NULL)" in function
assert function.index("if (data == NULL || image == NULL)") < function.index("cmts =")
assert "if (width == 0 || height == 0)" in function
assert function.index("if (width == 0 || height == 0)") < function.index("store the hints")
print("PASS XPM parser entry guard")
