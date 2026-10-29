#!/usr/bin/env python3
"""Check Toolbar child-creation failure paths avoid null widget calls."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Toolbar.c").read_text()
start = source.index("void XmToolbarSetActions")
end = source.index("Cardinal XmToolbarGetActionCount", start)
body = source[start:end]
assert "if (stored->button != NULL)" in body
assert "if (stored->menu_button != NULL)" in body
assert "if (toolbar->toolbar.actions[i].button == NULL)" in body
assert body.index("stored->button != NULL") < body.index("XtAddCallback")
assert body.index("stored->menu_button != NULL") < body.index("XtAddCallback", body.index("stored->menu_button != NULL"))
print("PASS Toolbar child-creation failure paths are null-safe")
