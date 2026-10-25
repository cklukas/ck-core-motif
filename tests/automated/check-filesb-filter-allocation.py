#!/usr/bin/env python3
"""Check that the legacy FileSelectionBox filter fallback is safe."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Obso1_2.c").read_text(encoding="utf-8")
start = source.index("_XmFileSelectionBoxCreateFilterText(")
end = source.index("\n/********************************************************", start)
region = source[start:end]
assert "stext_value = (char *) XtMalloc( 1)" in region
assert "if (stext_value == NULL)" in region
assert 'stext_value = "";' in region
assert "if (free_stext_value)" in region
print("PASS FileSelectionBox filter allocation is guarded")
