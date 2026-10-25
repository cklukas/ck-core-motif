#!/usr/bin/env python3
"""Check that XmFormSection footer width comparisons avoid underflow."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "FormSection.c").read_text(encoding="utf-8")
start = source.index("static void natural")
end = source.index("static void layout", start)
region = source[start:end]

assert "unsigned long inner_width" in region
assert "? *width - 2UL * section->section.margin_width : 0" in region
assert "(unsigned long)fw > inner_width" in region
print("PASS form-section margin comparisons are bounded")
