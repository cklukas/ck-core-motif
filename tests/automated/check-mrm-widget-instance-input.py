#!/usr/bin/env python3
"""Check Mrm widget-instance dispatch validates override and output pointers."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c").read_text(encoding="utf-8")
start = source.index("\nUrmCreateOrSetWidgetInstance (")
end = source.index("\n\n\f", start)
region = source[start:end]
guard = region.index("if ((ov_num_args > 0 && ov_args == NULL)")
dispatch = region.index("if (widgetrec->variety", guard)
assert guard < dispatch
assert "svlist == NULL" in region[guard:dispatch]
assert "w_return == NULL" in region[guard:dispatch]
assert "w_name == NULL" in region[guard:dispatch]
print("Mrm widget-instance dispatch validates override and output inputs")
