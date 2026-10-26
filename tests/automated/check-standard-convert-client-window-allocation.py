#!/usr/bin/env python3
"""Ensure standard client-window conversion handles allocation failure."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c"
text = source.read_text(encoding="utf-8")
start = text.index("  } else if (atoms[XmACLIENT_WINDOW] == cs -> target)")
end = text.index("  } else if (atoms[XmA_MOTIF_RENDER_TABLE] == cs -> target)", start)
block = text[start:end]

assert "cw = (Window *) XtMalloc(sizeof(Window));" in block
assert "if (cw == NULL)" in block
assert "cs -> type = XA_WINDOW;" in block
assert "_XmAppUnlock(app);" in block
assert "return;" in block

print("standard client-window conversion allocation guard: ok")
