#!/usr/bin/env python3
"""Ensure toolbar icons are applied to visible and overflow action buttons."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Toolbar.c").read_text(
    encoding="utf-8"
)
root = Path(__file__).parents[2]
header = (root / "lib/Xm/Toolbar.h").read_text(encoding="utf-8")
gallery = (root / "demos/programs/ckmotif-gallery/gallery_examples.c").read_text(encoding="utf-8")
assert "XmToolbarSetActionIcons" in header
assert "icons[i] != None" in source
assert "XmToolbarSetActionIcons(example, icons, 4)" in gallery
start = source.index("void XmToolbarSetActionIcons")
end = source.index("\nCardinal XmToolbarGetActionCount", start)
body = source[start:end]

assert "toolbar->toolbar.actions[i].menu_button" in body
assert "Widget targets[2]" in body
assert "XmNlabelPixmap" in body
assert "XmNlabelType, XmSTRING" in body
print("PASS Toolbar icons update visible and overflow actions")
