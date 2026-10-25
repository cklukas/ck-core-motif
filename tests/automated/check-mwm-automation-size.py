#!/usr/bin/env python3
"""Ensure MWM automation reply sizes use a string-capable counter."""

from pathlib import Path


source = Path(__file__).parents[2] / "clients" / "mwm" / "WmCmd.c"
text = source.read_text(encoding="utf-8")
start = text.index("GetAutomationData (")
end = text.index("\n}\n", start) + 3
function = text[start:end]

assert "  size_t size;" in function, "automation reply size must be size_t"
assert "  int size;" not in function, "automation reply size must not truncate to int"
assert "strlen(menu_item->label)" in function, "menu reply sizing must cover labels"
print("MWM automation reply sizes use size_t")
