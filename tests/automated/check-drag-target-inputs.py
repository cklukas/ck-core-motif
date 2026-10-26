#!/usr/bin/env python3
"""Check drag-target compatibility validates display and target arrays."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DragC.c").read_text(encoding="utf-8")
start = source.index("XmTargetsAreCompatible(")
region = source[start:]
assert "if (dpy == NULL ||" in region
assert "numExportTargets != 0 && exportTargets == NULL" in region
assert "numImportTargets != 0 && importTargets == NULL" in region
assert region.index("if (dpy == NULL ||") < region.index("_XmDisplayToAppContext(dpy)")
print("PASS XmTargetsAreCompatible validates display and target arrays")
