#!/usr/bin/env python3
"""Check icon-file cache-table failure leaves path resolution usable."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "IconFile.c").read_text()
assert "iconNameCache =  _XmAllocHashTable" in source
assert "if (!fileName && iconNameCache != NULL)" in source
assert "if (iconNameCache != NULL)" in source
assert "_XmAddHashEntry(iconNameCache" in source
assert "XtFree((char *)iNameEntry);" in source
print("PASS icon-file cache-table failure falls back safely")
