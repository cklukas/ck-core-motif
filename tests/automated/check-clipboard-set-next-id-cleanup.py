#!/usr/bin/env python3
"""Check the internal next-item ID setter releases its property buffer."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("static void \nClipboardSetNextItemId(")
end = source.index("static Boolean \nWeOwnSelection(", start)
body = source[start:end]

replace = body.index("ClipboardReplaceItem(")
release = body.index("XtFree( (char *)int_ptr );", replace)
assert release < body.index("}\n", release)

print("Clipboard next-item ID setter releases its property buffer")
