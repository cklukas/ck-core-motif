#!/usr/bin/env python3
"""Check public drop-transfer entry points validate caller inputs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DropTrans.c").read_text(encoding="utf-8")
start = source.index("XmDropTransferStart(")
add = source.index("XmDropTransferAdd(", start)
start_region = source[start:add]
add_region = source[add:]
assert "if (refWidget == NULL)" in start_region
assert start_region.index("if (refWidget == NULL)") < start_region.index("XtDisplayOfObject(refWidget)")
assert "if (widget == NULL || (num_transfers != 0 && transfers == NULL))" in add_region
assert add_region.index("if (widget == NULL || (num_transfers != 0 && transfers == NULL))") < add_region.index("_XmWidgetToAppContext(widget)")
print("PASS drop-transfer APIs validate widget and transfer inputs")
