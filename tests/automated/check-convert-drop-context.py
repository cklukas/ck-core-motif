#!/usr/bin/env python3
"""Check that external Motif-drop conversions require a live context."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("_XmConvertHandler(Widget wid")
end = source.index("\n/*ARGSUSED*/\nstatic void\nSecondaryConvertHandler", start)
region = source[start:end]
assert "*selection == atoms[XmA_MOTIF_DROP] && cc -> drag_context == NULL" in region
assert "!my_local_convert_flag" in region
assert region.index("local_convert_flag = 0") < region.index("cc -> drag_context == NULL")
assert region.index("cc -> drag_context == NULL") < region.index("XtGetValues(cc -> drag_context")
print("External Motif-drop conversions reject a missing context; local negotiation is allowed")
