#!/usr/bin/env python3
"""Check that RowColumn popup-shell name allocation is guarded."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "RowColumn.c").read_text(encoding="utf-8")
start = source.index("static Widget \ncreate(")
end = source.index("/*\n *************************************************************************\n *\n * Public Routines", start)
region = source[start:end]
assert "b = XtMalloc(strlen(POPUP_PREFIX) + strlen(name) + 1);" in region
assert "if (b == NULL)" in region
assert "sprintf (b, POPUP_PREFIX, name);" in region
print("PASS RowColumn popup name allocation is guarded")
