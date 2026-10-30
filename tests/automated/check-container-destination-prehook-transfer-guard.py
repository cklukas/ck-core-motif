#!/usr/bin/env python3
"""Check ContainerDestPrehookProc rejects callbacks without a transfer id."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.index("ContainerDestPrehookProc(\n        Widget", source.index("ContainerDestPrehookProc (Trait Method)"))
end = source.index("FreeLocationData(", start)
region = source[start:end]
guard = "if (wid == NULL || cs == NULL || cs->transfer_id == NULL)"
assert guard in region
assert region.index(guard) < region.index("XtDisplay(wid)")
assert region.index(guard) < region.index("XmTransferDone(")
print("PASS ContainerDestPrehookProc rejects callbacks without a transfer id")
