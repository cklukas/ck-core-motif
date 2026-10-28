#!/usr/bin/env python3
"""Check that transfer selection callbacks validate their callback state."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("SelectionCallbackWrapper(")
end = source.index("/**********************************************************/\n/* Context block handlers", start)
region = source[start:end]
assert "wid == NULL" in region
assert "tc == NULL" in region
assert "selection == NULL" in region
assert "length == NULL" in region
guard = region[region.index("if (wid == NULL"):region.index("tb =", region.index("if (wid == NULL"))]
assert "XtFree((char *)value);" in guard
assert "tc -> widget == NULL" in region
assert region.count("XtFree((char *)value);") >= 3
assert "tb = tc -> requests" in region
print("selection callbacks reject incomplete transfer state")
