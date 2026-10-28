#!/usr/bin/env python3
"""Check ComboBox selected-position export guards a missing array."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ComboBox.c").read_text()
start = source.index("static void\nCBGetSelectedPos")
end = source.index("\n}\n", start)
body = source[start:end]

assert "result = (count > 0 && pos != NULL) ? *pos : 0;" in body
assert "result = (count > 0) ? *pos : 0;" not in body

print("ComboBox selected-position export guards a missing array")
