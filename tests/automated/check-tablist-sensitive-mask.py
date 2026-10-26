#!/usr/bin/env python3
"""Check XmTabbedStackListInsert tests the sensitivity bit as a mask."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TabList.c").read_text(
    encoding="utf-8"
)
start = source.index("XmTabbedStackListInsert(")
end = source.index("XmTabbedStackListAppend(", start)
region = source[start:end]

assert "if( mask & XmTAB_SENSITIVE )" in region
assert "mask * XmTAB_SENSITIVE" not in region
print("XmTabbedStackListInsert applies the sensitivity bit mask correctly")
