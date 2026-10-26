#!/usr/bin/env python3
"""Check bounded XmIconLabel geometry calculations."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "IconLabel.c").read_text(encoding="utf-8")
assert "label->icon_label.render_table && label->icon_label.label" in source
dimensions = source.index("static void dimensions")
end_dimensions = source.index("static void Initialize", dimensions)
region = source[dimensions:end_dimensions]
assert "total_width > USHRT_MAX" in region
assert "total_height > USHRT_MAX" in region
assert "value && label->icon_label.render_table" in source
assert "XmString replacement = copy_label" in source
print("PASS icon-label geometry is bounded")
