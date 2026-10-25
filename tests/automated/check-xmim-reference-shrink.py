#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/XmIm.c").read_text()
start = source.index("static Cardinal\nremove_ref")
end = source.index("\n/* Convert a VaArgList", start)
body = source[start:end]
assert "Widget *new_refs" in body
assert "XIMCallback **new_callbacks" in body
assert "if (new_refs != NULL && new_callbacks != NULL)" in body
assert "memcpy(new_refs" in body
assert "refs->max_refs = new_max;" in body
print("XmIm reference-list shrink guards present")
