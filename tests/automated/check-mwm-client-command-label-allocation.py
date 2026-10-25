#!/usr/bin/env python3
"""Check that parsed MWM client-command labels handle copy failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResParse.c").read_text(encoding="utf-8")
start = source.rindex("static Boolean ParseClientCommand")
end = source.index("\n}\n", start) + 2
region = source[start:end]
assert "menuItem->label = XtNewString(unchanged_stream);" in region
assert "if (menuItem->label == NULL)" in region
assert "XtFree((char *)unchanged_stream);" in region
assert "return_val = FALSE;" in region
print("PASS MWM client-command label copying is guarded")
