#!/usr/bin/env python3
"""Check oversized toolbar input is rejected before existing actions are freed."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Toolbar.c").read_text()
start = source.index("void XmToolbarSetActions")
end = source.index("void XmToolbarSetActionIcons", start)
function = source[start:end]
guard = function.index("SIZE_MAX / sizeof(*toolbar->toolbar.actions)")
free = function.index("free_actions(toolbar, True)")
assert guard < free
assert "(count && !actions)" in function
print("Toolbar rejects oversized action arrays before replacing existing actions")
