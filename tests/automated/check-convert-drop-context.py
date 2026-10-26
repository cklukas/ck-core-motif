#!/usr/bin/env python3
"""Check that Motif-drop conversion requires a live drag context."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("_XmConvertHandler(Widget wid")
end = source.index("\n/*ARGSUSED*/\nstatic void\nSecondaryConvertHandler", start)
region = source[start:end]
assert "*selection == atoms[XmA_MOTIF_DROP] && cc -> drag_context == NULL" in region
assert region.index("cc -> drag_context == NULL") < region.index("XtGetValues(cc -> drag_context")
print("Motif-drop conversion rejects a missing drag context")
