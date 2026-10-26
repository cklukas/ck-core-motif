#!/usr/bin/env python3
"""Check the shared varargs widget creator validates its parent first."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "VaSimple.c").read_text(
    encoding="utf-8"
)
start = source.rindex("XmeVLCreateWidget(")
body_start = source.index("{", start)
body_end = source.index("\n}", body_start)
body = source[body_start:body_end]
guard = "if (parent == (Widget) NULL)\n        return NULL;"
assert guard in body
assert body.index(guard) < body.index("_XmWidgetToAppContext(parent)")
print("shared varargs widget creation guards null parents before context lookup")
