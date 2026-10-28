#!/usr/bin/env python3
"""Check external TARGETS counts are aligned and int-representable."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("int \nXmClipboardInquireCount(")
end = source.index("int \nXmClipboardInquireFormat(", start)
body = source[start:end]
cast = body.index("loc_count = (int) loc_count_len")
guard = body.index("loc_count_len % sizeof(Atom)", 0, cast)
assert guard < cast
assert "loc_count_len / sizeof(Atom) > (unsigned long)INT_MAX" in body
assert body.index("XtFree( (char *)alloc_to_free );", guard) < body.index("return ClipboardFail;", guard)

print("Clipboard inquiry counts reject malformed or oversized TARGETS")
