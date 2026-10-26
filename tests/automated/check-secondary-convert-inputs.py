#!/usr/bin/env python3
"""Check that secondary conversion validates its callback payload."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("SecondaryConvertHandler(Widget w")
end = source.index("\n/*ARGSUSED*/\nstatic void\nReleaseSecondaryLock", start)
region = source[start:end]
assert "if (w == NULL || cs == NULL)" in region
assert "cs -> parm == NULL || cs -> parm_length < 2" in region
guard = region.index("if (w == NULL || cs == NULL)")
assert guard < region.index("XtWidgetToApplicationContext(w)")
assert region.index("cs -> parm == NULL") < region.index("XtGetSelectionRequest")
print("secondary conversion validates widget, callback, and pair payload")
