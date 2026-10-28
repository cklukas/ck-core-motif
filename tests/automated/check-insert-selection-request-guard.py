#!/usr/bin/env python3
"""Check insert-selection conversion handles a missing request event."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataFSel.c").read_text()
start = source.index("ConvertInsertSelection(")
end = source.index("/*\n   * Make sure the above selection request", start)
body = source[start:end]

assert "req_event = XtGetSelectionRequest" in body
assert "if (req_event == NULL)" in body
assert "return FALSE;" in body

print("Insert-selection conversion guards missing request events")
