#!/usr/bin/env python3
"""Check that MWM command-tree path construction checks allocation."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmMenu.c").read_text(encoding="utf-8")
start = source.rindex("static void InsertTreeOnClient")
end = source.index("\n}\n", start) + 2
region = source[start:end]
assert "size_t length;" in region
assert "new_command_str = XtMalloc((Cardinal) length);" in region
assert "if (new_command_str == (String)NULL)" in region
assert "snprintf(new_command_str, length" in region
print("PASS MWM command-tree path allocation is guarded")
