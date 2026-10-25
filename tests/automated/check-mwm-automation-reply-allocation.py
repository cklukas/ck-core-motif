#!/usr/bin/env python3
"""Check that all MWM automation reply allocations are checked."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmCmd.c").read_text(encoding="utf-8")
start = source.index("GetAutomationData (")
end = source.index("\n}", start)
region = source[start:end]

assert "#define ALLOC_AUTOMATION_REPLY()" in region
assert "size > (size_t)-1 / sizeof(CARD8)" in region
assert "if (*output == NULL)" in region
assert "automation_alloc_failed:" in region
assert "XtMalloc(sizeof(CARD8) * size)" in region
assert region.count("*output = (XtPointer)XtMalloc(sizeof(CARD8) * size);") == 1
print("PASS MWM automation reply allocations are checked")
