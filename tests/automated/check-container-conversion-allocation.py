#!/usr/bin/env python3
"""Check Container conversion allocation failure paths."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text()
start = source.index("ContainerConvertProc(")
start = source.index("ContainerConvertProc(", start + 1)
end = source.index("\n/************************************************************************\n * ContainerDestinationProc", start)
region = source[start:end]

for needle in (
    "if (targargs == NULL)",
    "if (offset_args == NULL)",
    "if (items == NULL)",
    "if (return_pm == NULL)",
    "ConvertRefuse(wid, closure, cs);",
):
    assert needle in region, needle
assert "if ((item_count = cw->container.selected_item_count) == 0)" in region
print("Container conversion handles allocation failure")
