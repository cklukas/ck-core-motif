#!/usr/bin/env python3
"""Check modal-grab table growth is bounded and failure-safe."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/VendorS.c").read_text(
    encoding="utf-8"
)

assert "Cardinal old_max = xmDisplay->display.maxModals;" in source
assert "SIZE_MAX / sizeof(XmModalDataRec)" in source
assert "UINT_MAX / sizeof(XmModalDataRec)" in source
assert "if (resized == NULL)" in source
assert "xmDisplay->display.maxModals = new_max;" in source
assert "xmDisplay->display.maxModals +=  (xmDisplay->display.maxModals / 2) + 2" not in source
print("Vendor-shell modal table growth is bounded and failure-safe")
