#!/usr/bin/env python3
from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmWinInfo.c"
text = source.read_text()
start = text.index("void \nProcessSmClientID")
body = text[start:text.index("/* END OF FUNCTION ProcessSmClientID", start)]

assert "actualFormat == 8" in body
assert "nitems > 0" in body
assert "XtMalloc((Cardinal)nitems + 1)" in body
assert "pCD->smClientID[nitems] = '\\0'" in body
assert "XFree(clientID)" in body
print("ProcessSmClientID validates, copies, and terminates the property string")
