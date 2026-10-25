#!/usr/bin/env python3
"""Check the additive borrowed-pixmap toolbar icon API."""
from pathlib import Path

root = Path(__file__).parents[2]
header = (root / "lib/Xm/Toolbar.h").read_text(encoding="utf-8")
source = (root / "lib/Xm/Toolbar.c").read_text(encoding="utf-8")
gallery = (root / "demos/programs/ckmotif-gallery/gallery_examples.c").read_text(encoding="utf-8")
assert "XmToolbarSetActionIcons" in header
assert "icons[i] != None" in source
assert "XmNlabelPixmap" in source
assert "XmToolbarSetActionIcons(example, icons, 4)" in gallery
print("PASS toolbar supports optional native action icons")
