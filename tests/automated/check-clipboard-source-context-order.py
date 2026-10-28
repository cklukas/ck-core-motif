#!/usr/bin/env python3
"""Check clipboard-source context replacement follows ownership success."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("XmeClipboardSource(Widget w, XtEnum op, Time time)")
end = source.index("/********************************************************", start)
body = source[start:end]

owner = body.index("clipboard_owner = XGetSelectionOwner")
own = body.index("selection_status = XtOwnSelection")
assert body.index("if (clipboard_owner == None)") < body.index("LookupContextBlock")
clear = body.index("ClearContextBlock", own)
assert clear > own
assert "cc = LookupContextBlock(display, atoms[XmACLIPBOARD]);" in body[clear:]

print("PASS clipboard source preserves context until manager ownership succeeds")
