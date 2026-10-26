#!/usr/bin/env python3
"""Check XmImGetXICResetState handles a null widget safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmIm.c").read_text(encoding="utf-8")
start = source.index("XmImGetXICResetState(Widget w)")
region = source[start:]
assert "if (w == NULL)" in region
assert region.index("if (w == NULL)") < region.index("get_current_xic(get_xim_info(w), w)")
print("PASS XmImGetXICResetState validates null widgets")
