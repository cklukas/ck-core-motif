#!/usr/bin/env python3
"""Check that MWM rectangle-list allocation bounds are explicit."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmGraphics.c").read_text(encoding="utf-8")
extend = source[source.index("Boolean ExtendRList"):source.index("} /* END OF FUNCTION ExtendRList */")]
allocate = source[source.index("RList *AllocateRList"):source.index("} /* END OF FUNCTION AllocateRList */")]
assert "prl->allocated < 0" in extend
assert "amt > INT_MAX" in extend
assert "amt > UINT_MAX / sizeof(XRectangle)" in extend
assert "XtMalloc ((Cardinal)(total * sizeof(XRectangle)))" in extend
assert "amt > INT_MAX" in allocate
assert "amt > UINT_MAX / sizeof(XRectangle)" in allocate
assert "XtMalloc ((Cardinal)(amt * sizeof(XRectangle)))" in allocate
print("PASS MWM rectangle-list allocation bounds are explicit")
