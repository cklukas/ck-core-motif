#!/usr/bin/env python3
from pathlib import Path

source = Path(__file__).parents[2] / "lib/Xm/DataFSel.c"
text = source.read_text()
start = text.index("ConvertInsertSelection(\n", 3000)
body = text[start:text.index("/* ARGSUSED */", start)]

assert '"ATOM_PAIR"' in body
assert "actual_format != 32" in body
assert "nitems < 2" in body
assert "prop == NULL" in body
assert "XFree(prop)" in body
assert "pair = (_XmTextInsertPair *)prop" in body
print("ConvertInsertSelection validates ATOM_PAIR property shape before casting")
