#!/usr/bin/env python3
from pathlib import Path
text=(Path(__file__).parents[2]/"lib"/"Xm"/"GMUtils.c").read_text()
assert "if (widget == NULL || desired == NULL)" in text
assert "intended != NULL && (IsWidth(intended))" in text
print("Xm geometry reply validates null inputs")
