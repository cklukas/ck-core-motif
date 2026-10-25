#!/usr/bin/env python3
"""Check that legacy Motif resource-string copies handle allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Obso1_2.c").read_text(encoding="utf-8")
restore = source.index("_XmFileSelectionBoxRestore(")
restore_end = source.index("/*****************************************************************/", restore + 1)
restore_region = source[restore:restore_end]
assert "if (itemString != NULL)" in restore_region
assert "XtFree( mask)" in restore_region
getter = source.index("_XmRC_GetMenuAccelerator(")
getter_end = source.index("/*ARGSUSED*/", getter + 1)
getter_region = source[getter:getter_end]
assert "if (data != NULL)" in getter_region
assert "*value = (XtArgVal) data" in getter_region
print("PASS legacy resource-string allocations are guarded")
