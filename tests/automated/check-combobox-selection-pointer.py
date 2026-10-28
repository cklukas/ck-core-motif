#!/usr/bin/env python3
"""Check ComboBox navigation validates the selected-position pointer."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ComboBox.c").read_text()
start = source.index("static void\nCBListAction")
end = source.index("\n}\n", start)
body = source[start:end]

assert "if (count && pos != NULL)" in body
assert "if (count)" not in body

print("ComboBox navigation guards a missing selected-position array")
