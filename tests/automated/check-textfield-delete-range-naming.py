#!/usr/bin/env python3
"""Check the delete-target range does not shadow conversion ranges."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextFSel.c").read_text(
    encoding="utf-8"
)
start = source.index("} else if (*target == atoms[XmADELETE])")
end = source.index("\n  } else if", start + 1)
body = source[start:end]
assert "XmTextPosition delete_left, delete_right;" in body
assert "delete_left = tf->text.prim_pos_left;" in body
assert "delete_right = tf->text.prim_pos_right;" in body
assert "delete_left, delete_right, NULL" in body
print("PASS TextField delete-target range names remain distinct")
