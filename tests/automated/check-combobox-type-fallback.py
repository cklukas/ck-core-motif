#!/usr/bin/env python3
"""Check ComboBox type setup does not assert on invalid values."""

from pathlib import Path


source = Path("lib/Xm/ComboBox.c").read_text()
start = source.index("/* Setup all the accelerators and translations. */")
end = source.index("/* Install the standard ComboBox accelerators everywhere. */", start)
region = source[start:end]

assert "case XmCOMBO_BOX:" in region
assert "case XmDROP_DOWN_COMBO_BOX:" in region
assert "assert(False);" not in region
assert "default:" in region

print("ComboBox invalid type setup falls back safely")
