#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/DialogS.c").read_text()
start = source.index("XmeCreateClassDialog(")
end = source.index("/*\tCreate the widget.", start)
region = source[start:end]
assert "if (bb_n && !bb_args)" in region
assert "if (!ds_name)" in region
assert "if (!ds_args)" in region
assert "if (!ds)" in region
print("Class dialog allocation guards present")
