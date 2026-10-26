#!/usr/bin/env python3
"""Check that conversion callback parameter fields start deterministic."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("_XmConvertHandler(Widget wid")
end = source.index("\n/* DragConvertHandler", start)
region = source[start:end]
for line in (
    "cbstruct.parm = NULL;",
    "cbstruct.parm_length = 0;",
    "cbstruct.parm_format = 8;",
    "cbstruct.parm_type = None;",
):
    assert line in region
assert region.index("cbstruct.parm = NULL;") < region.index("XtGetSelectionParameters")
print("conversion callback parameter fields have deterministic defaults")
