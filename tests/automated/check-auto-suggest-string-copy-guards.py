#!/usr/bin/env python3
"""Check safe AutoSuggestBox XmString copying."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "AutoSuggestBox.c").read_text(encoding="utf-8")
start = source.index("static XmString string_copy")
end = source.index("static void free_items", start)
region = source[start:end]

assert "if (!text)" in region
assert "XmStringCreateLocalized(text)" in region
assert "if (!box->auto_suggest.items)" in source
assert "if (!box->auto_suggest.items[i].id || !label)" in source
assert "box->auto_suggest.item_count = i + 1" in source
print("PASS AutoSuggestBox string copies validate conversion allocation")
