#!/usr/bin/env python3
"""Check secondary-selection completion rejects malformed payloads."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("SecondaryDone(Widget wid")
end = source.index("Boolean\nXmeClipboardSource", start)
body = source[start:end]

assert "*format != 8 && *format != 16 && *format != 32" in body
assert "*length != 0 && (*type == None || value == NULL)" in body
reject = body.index("XtFree((char *)value);", body.index("else if"))
assert reject < body.index("XtDisownSelection(wid, XA_SECONDARY", reject)

print("PASS secondary completion rejects malformed payloads")
