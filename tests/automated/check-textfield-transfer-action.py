#!/usr/bin/env python3
"""Check TextField transfer-action allocation is transactional."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text()
start = source.index("static void\nProcessBSelect")
end = source.index("\nstatic void", start + 20)
body = source[start:end]

assert "new_action =" in body
assert "new_event = (XEvent *)XtMalloc" in body
assert "if (new_action == NULL || new_event == NULL)" in body
assert "XtFree((char *)new_action);" in body
assert "if (tf->text.transfer_action->event == NULL)" in body

print("TextField transfer-action allocation is transactional")
