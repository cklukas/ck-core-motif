#!/usr/bin/env python3
"""Check SearchBox placeholder SetValues ownership and failure handling."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib/Xm/SearchBox.c"
text = source.read_text(encoding="utf-8")

start = text.index("static Boolean SetValues")
end = text.index("static void query_changed", start)
body = text[start:end]
assert "XmString replacement;" in body
assert "replacement = copy_label" in body
assert "if (replacement)" in body
assert "XmStringFree(old->search.placeholder);" in body
assert "box->search.placeholder = old->search.placeholder;" in body
print("SearchBox placeholder ownership is guarded")
