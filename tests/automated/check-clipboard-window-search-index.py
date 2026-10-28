#!/usr/bin/env python3
"""Check clipboard descendant traversal matches XQueryTree's count width."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("static int \nClipboardSearchForWindow(")
end = source.index("static int \nClipboardWindowExists(", start)
body = source[start:end]

assert "unsigned int i;" in body
assert "for ( i = 0; i < numchildren; i++ )" in body

print("Clipboard window search preserves child-count width")
