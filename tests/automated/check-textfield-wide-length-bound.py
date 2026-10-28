#!/usr/bin/env python3
"""Check TextField wide-transfer sizing rejects INT_MAX before +1."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextFSel.c").read_text(
    encoding="utf-8"
)
start = source.index("wc_total_length = _XmTextFieldCountCharacters")
body = source[start : source.index("\n    wc_total_value", start)]
assert "wc_total_length == INT_MAX" in body
allocation = source.index("(wc_total_length+1)", start)
assert source.index("wc_total_length == INT_MAX", start) < allocation
print("PASS TextField wide transfer rejects INT_MAX before sizing +1")
