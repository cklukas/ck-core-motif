#!/usr/bin/env python3
"""Check ComboBox deletion guards a missing selected-position array."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ComboBox.c").read_text()
start = source.index("XmComboBoxDeletePos")
end = source.index("\n}\n", start)
body = source[start:end]

assert "selpos = (count > 0 && positions != NULL) ? *positions : 0;" in body
assert "selpos = (count > 0) ? *positions : 0;" not in body

print("ComboBox deletion guards a missing selected-position array")
