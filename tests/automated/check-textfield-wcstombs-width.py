#!/usr/bin/env python3
"""Check TextField selection conversions retain wcstombs size_t results."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextFSel.c").read_text(
    encoding="utf-8"
)
start = source.index("_XmTextFieldConvert(")
end = source.index("\n/* ARGSUSED */\nvoid\n_XmTextFieldLoseSelection", start)
body = source[start:end]
assert body.count("size_t converted;") >= 4
assert body.count("converted = wcstombs") >= 4
assert body.count("converted == (size_t)-1") >= 4
assert "int stat" not in body
print("PASS TextField wcstombs results retain size_t width")
