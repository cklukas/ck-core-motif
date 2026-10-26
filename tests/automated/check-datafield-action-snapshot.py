#!/usr/bin/env python3
"""Check DataField action event snapshots handle allocation failure."""

from pathlib import Path


source = Path("lib/Xm/DataF.c").read_text(encoding="utf-8")

assert "if (tmp == NULL || event == NULL)" in source
assert "if (tmp->event == NULL)" in source
assert "XtFree((char *)tmp);\n    return;" in source

print("DataField action-snapshot checks passed")
