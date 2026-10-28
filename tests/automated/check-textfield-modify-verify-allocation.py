#!/usr/bin/env python3
"""Check TextField modify-verify temporary buffers handle failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text()
start = source.index("snapshot = XtMalloc")
end = source.index("      if (!mod_ver_ret)", start)
body = source[start:end]

assert "proposed_length < 0" in source[start - 450:start]
assert "if (snapshot == NULL)" in body
assert "if (approved != NULL)" in body
assert "insert_length >= 0" in body
assert body.index("if (approved != NULL)") < body.index("XtFree(proposed);")
print("TextField modify-verify allocations are guarded")
