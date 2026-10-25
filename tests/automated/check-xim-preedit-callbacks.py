#!/usr/bin/env python3
"""Check XIM preedit callbacks guard missing client data."""

from pathlib import Path


source = Path("lib/Xm/XmIm.c").read_text()
start = source.index("ImPreeditStartCallback(", source.index("static int\nImPreeditStartCallback") + 1)
end = source.index("ImPreeditDoneCallback(", start)
start_callback = source[start:end]
start = source.index("ImPreeditDoneCallback(", start)
end = source.index("ImPreeditDrawCallback(", start)
done_callback = source[start:end]

assert "if (!client_data)" in start_callback
assert "return 0;" in start_callback
assert "if (!client_data)" in done_callback
assert "return;" in done_callback
assert "assert(False);" not in start_callback + done_callback

print("XIM preedit callbacks guard missing client data")
