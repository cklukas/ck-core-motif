#!/usr/bin/env python3
"""Check ColorPicker focus geometry fits small widgets."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ColorPicker.c").read_text(encoding="utf-8")
start = source.index("if (picker->primitive.highlighted")
end = source.index("\n    }", start)
region = source[start:end]
assert "picker->core.width > 2 * picker->primitive.highlight_thickness" in region
assert "picker->core.height > 2 * picker->primitive.highlight_thickness" in region
print("color-picker highlight geometry is bounded")
