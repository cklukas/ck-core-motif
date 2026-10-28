#!/usr/bin/env python3
"""Check external format inquiries release TARGETS buffers on no-data results."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("int \nXmClipboardInquireFormat(")
end = source.index("int \nXmClipboardInquireLength(", start)
body = source[start:end]
external = body[body.index("if ( !ClipboardGetSelection"):body.index("if ( ptr != 0 )")]

assert "if ( loc_matchlength >= n )" in external
release = external.index("XtFree( (char *) alloc_to_free );")
assert release > external.index("if ( loc_matchlength >= n )")
assert release > external.rindex("}") - 80

print("Clipboard format inquiry releases external TARGETS buffers")
