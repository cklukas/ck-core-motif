#!/usr/bin/env python3
"""Check ComboBox Up navigation never passes zero to the one-based List API."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ComboBox.c").read_text()
start = source.index("static void\nCBListAction")
end = source.index("\n}\n", start)
body = source[start:end]

assert "if (*pos > 1)" in body
assert "XmListSelectPos(CB_List(cb), *pos - 1, TRUE);" in body
assert "if (*pos >= 1)" not in body

print("ComboBox Up navigation preserves the first one-based item")
