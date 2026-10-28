#!/usr/bin/env python3
"""Check ComboBox selection-to-text sync validates List arrays and positions."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ComboBox.c").read_text()
start = source.index("/* CR 8445:")
end = source.index("\n    _XmAppUnlock(app);", start)
body = source[start:end]

assert "pcount > 0 && pos != NULL && icount > 0 && items != NULL" in body
assert "pos[0] > 0 && pos[0] <= icount" in body
assert "items[pos[0] - 1]" in body

print("ComboBox selection-to-text sync validates List arrays and positions")
