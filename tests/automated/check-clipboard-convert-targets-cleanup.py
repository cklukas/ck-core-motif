#!/usr/bin/env python3
"""Check target-list setup releases its first format on early failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("static Boolean\nClipboardConvertProc(")
end = source.index("static int \nClipboardDataIsReady(", start)
body = source[start:end]
start_targets = body.index("nextitem = ClipboardFindFormat")
targets = body[start_targets:body.index("} else if ( *target == atoms[XmA_TIMESTAMP] )", start_targets)]

assert targets.count("XtFree( (char *)nextitem );") == 2
assert "XtFree( (char *) nextitem );" in targets
assert "if (ptr == NULL)" in targets

print("Clipboard target-list setup releases format records on failure")
