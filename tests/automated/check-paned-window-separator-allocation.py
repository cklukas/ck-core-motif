#!/usr/bin/env python3
"""Check that PanedWindow separator collection is bounded and guarded."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "PanedW.c").read_text()
start = source.index("if (oldpw->paned_window.separator_on")
end = source.index("\n   if(!XmRepTypeValidValue", start)
region = source[start:end]

assert "SIZE_MAX / sizeof(*sep_children)" in region
assert "UINT_MAX / sizeof(*sep_children)" in region
assert "XtMalloc(\n             (Cardinal)((size_t)num_children *" in region
assert "if (sep_children != NULL)" in region
assert region.index("if (sep_children != NULL)") < region.index("sep_children[num_separators]")
assert "num_children/3" not in region

print("PASS PanedWindow separator allocation is bounded and guarded")
