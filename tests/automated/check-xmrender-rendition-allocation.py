#!/usr/bin/env python3
"""Check that XmRenderT rendition cloning handles allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c").read_text(encoding="utf-8")
clone = source.index("CloneRendition(XmRendition rend)\n{")
renew = source.index("RenewRendition(XmRendition rend)\n{", clone)
clone_region = source[clone:renew]
renew_end = source.index("/* Allocate a new handle", renew)
renew_region = source[renew:renew_end]
assert "if (copy == NULL)" in clone_region
assert "if (copy_handle == NULL)" in clone_region
assert "XtFree((char *)copy);" in clone_region
assert "if (copy == NULL)" in renew_region
print("PASS XmRenderT rendition allocations are guarded")
