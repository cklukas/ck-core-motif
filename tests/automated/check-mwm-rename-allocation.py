#!/usr/bin/env python3
"""Check that MWM command renames preserve the old label on failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmMenu.c").read_text(encoding="utf-8")
start = source.index("static Boolean ModifyClientCommandForMenuSpec")
end = source.index("\n}\n", start) + 2
region = source[start:end]
assert "String replacement;" in region
assert "replacement = XtNewString(newname);" in region
assert "if (replacement == NULL)" in region
assert "XtFree(curMenuItem->label);" in region
assert "curMenuItem->label = replacement;" in region
print("PASS MWM command rename allocation is transactional")
