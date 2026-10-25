#!/usr/bin/env python3
"""Check Form fallback offsets do not wrap unsigned widget dimensions."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Form.c").read_text(encoding="utf-8")
assert source.count("AssignValue(a, (int)fw->core.width - GetFormOffset(fw,which,att));") == 2
assert source.count("AssignValue(a, (int)fw->core.height - GetFormOffset(fw,which,att));") == 2
assert "AssignValue(a, fw->core.width - GetFormOffset(fw,which,att));" not in source
assert "AssignValue(a, fw->core.height - GetFormOffset(fw,which,att));" not in source
print("form fallback offsets use signed widget dimensions")
