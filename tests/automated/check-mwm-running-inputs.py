#!/usr/bin/env python3
"""Check XmIsMotifWMRunning validates its shell before XtScreen."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "IsMwmRun.c").read_text(encoding="utf-8")
start = source.index("XmIsMotifWMRunning(")
region = source[start:]
assert "if (shell == NULL)" in region
assert region.index("if (shell == NULL)") < region.index("XtScreen(shell)")
print("PASS XmIsMotifWMRunning validates null shells")
