#!/usr/bin/env python3
"""Check Toolbar action-array allocation failure cannot reach a null dereference."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Toolbar.c").read_text()
start = source.index("void XmToolbarSetActions")
body = source[start:source.index("void XmToolbarSetActionIcons", start)]
assert "SIZE_MAX / sizeof(*toolbar->toolbar.actions)" in body
assert "UINT_MAX / sizeof(*toolbar->toolbar.actions)" in body
assert "replacement == NULL" in body
assert body.index("replacement == NULL") < body.index("toolbar->toolbar.actions = replacement")
assert "unable to allocate action array" in body
print("PASS Toolbar action-array allocation failure handling")
