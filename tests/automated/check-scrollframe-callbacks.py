#!/usr/bin/env python3
"""Check that navigator callbacks are validated before ScrollFrame use."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ScrollFrameT.c").read_text()
start = source.index("_XmSFAddNavigator(")
end = source.index("_XmSFRemoveNavigator(", start)
function = source[start:end]

assert "nav_trait->setValue == NULL" in function
assert "nav_trait->changeMoveCB == NULL" in function
assert function.index("XmeWarning(sf, SWMessage3)") < function.index("changeMoveCB(nav")
assert function.index("XmeWarning(sf, SWMessage3)") < function.index("setValue(nav")
print("PASS ScrollFrame navigator callbacks are validated")
