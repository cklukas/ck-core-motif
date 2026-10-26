#!/usr/bin/env python3
"""Check that invalid snapshot results release their returned buffer."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("if (value == NULL || size2 != 1 || format2 != 32)")
end = source.index("\n      }", start) + 8
region = source[start:end]
assert "XtFree((char *)value);" in region
assert region.index("XtFree((char *)value);") < region.index("XtFree((char *)targets);")
print("invalid clipboard snapshot results release their returned buffer")
