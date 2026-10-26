#!/usr/bin/env python3
"""Check that MWM automation replies bound every size accumulation."""
from pathlib import Path


source = (Path(__file__).parents[2] / "clients/mwm/WmCmd.c").read_text(
    encoding="utf-8"
)
start = source.index("GetAutomationData (XtPointer")
end = source.index("\nautomation_alloc_failed:", start)
region = source[start:end]

assert "#define ADD_AUTOMATION_SIZE(extra)" in region
assert "addition > SIZE_MAX - size" in region
assert "addition > (size_t)UINT_MAX - size" in region
assert "#define ALLOC_AUTOMATION_REPLY()" in region
assert "if (size > UINT_MAX)" in region
assert "XtMalloc((Cardinal)size)" in region
assert region.count("size +=") == 1
assert "strlen(menu_item->label)" in region
assert "ADD_AUTOMATION_SIZE(sizeof (CARD16) +" in region
print("PASS MWM automation reply sizes are checked before allocation")
