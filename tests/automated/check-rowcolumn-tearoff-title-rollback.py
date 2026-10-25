#!/usr/bin/env python3
"""Check RowColumn tear-off title replacement is transactional."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "RowColumn.c").read_text()
start = source.index("/* Copy new value,  free old value")
end = source.index("\n   if (IsBar", start)
region = source[start:end]
assert "XmString replacement;" in source
assert "if (RC_TearOffTitle(new_w) == NULL)" in region
assert "replacement = XmStringCopy(RC_TearOffTitle(new_w));" in region
assert "if (replacement != NULL)" in region
assert "RC_TearOffTitle(new_w) = RC_TearOffTitle(old);" in region
print("RowColumn tear-off titles preserve current values on copy failure")
