#!/usr/bin/env python3
"""Check that a cleared secondary context still releases callback data."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("SecondaryDone(Widget wid")
end = source.index("\n\n\n/****************************************************************/\n/* XmeClipboardSource", start)
region = source[start:end]
branch = region[region.index("if (cc == NULL)"):]
assert "XtFree((char *)value);" in branch
assert branch.index("XtFree((char *)value);") < branch.index("XtDisownSelection")
print("secondary conversion cleanup releases data after context teardown")
