#!/usr/bin/env python3
from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmWinInfo.c"
text = source.read_text()
start = text.index("void \nProcessWmSaveHint")
body = text[start:text.index("/* END OF FUNCTION ProcessWmSaveHint", start)]

assert "actualFormat == 32" in body
assert "nitems > 0" in body
assert "*saveHintFlags" in body
print("ProcessWmSaveHint validates a non-empty 32-bit property before dereference")
