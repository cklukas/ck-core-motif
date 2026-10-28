#!/usr/bin/env python3
"""Check that Notebook SetValues initializes its optional geometry reply."""
from pathlib import Path
import sys

source = Path(sys.argv[1]) if len(sys.argv) == 2 else \
    Path(__file__).parents[2] / "lib" / "Xm" / "Notebook.c"
if len(sys.argv) > 2:
    raise SystemExit(f"usage: {sys.argv[0]} [NOTEBOOK_SOURCE]")
text = source.read_text(encoding="utf-8")
expected = "XtWidgetGeometry preferred_geo = { 0 };"
assert expected in text, "Notebook preferred geometry must be initialized"
assert "NewPreferredGeometry(new_w, NULL, NULL, &preferred_geo);" in text
print("PASS Notebook preferred geometry has a defined fallback state")
