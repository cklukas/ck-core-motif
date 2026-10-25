#!/usr/bin/env python3
"""Check SpinBox textual trait callbacks and widget inputs are guarded."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "SpinB.c").read_text()
assert "if (textW == NULL)" in source
assert "textT == NULL || textT->setValue == NULL" in source
assert "textT == NULL || textT->getValue == NULL" in source
print("PASS SpinBox textual trait callbacks are guarded")
