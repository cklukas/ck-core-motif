#!/usr/bin/env python3
"""Check Container detail-order copies handle allocation failure."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "Container.c"
text = source.read_text()
if text.count("if (detail_order == NULL)") < 1:
    raise SystemExit("resource-update detail-order allocation is not guarded")
if "if (ncw->container.detail_order == NULL)" not in text:
    raise SystemExit("initial detail-order allocation is not guarded")
if text.count("detail_order_count = 0") < 4:
    raise SystemExit("detail-order failure paths do not clear the count")

print("Container detail-order copies guard allocation failures")
