#!/usr/bin/env python3
"""Check retrieve releases the matching format after persisting it."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("static int\nClipboardRetrieve(")
end = source.index("int \nXmClipboardInquireCount(", start)
body = source[start:end]
replace = body.index("ClipboardReplaceItem(display, \n\t\t\t\t matchid")
release = body.index("XtFree( (char *) matchformat );", replace)
assert release < body.index("}\n\n\n\t/* Unfortunately", release)

print("Clipboard retrieve releases the matching format record")
