#!/usr/bin/env python3
"""Check that inline command-tree names are dynamically formatted."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmMenu.c").read_text()
start = source.index("static void SearchForOtherMatches")
end = source.index("\n    /* Search through all of the global menuspecs also. */", start)
region = source[start:end]
assert "char new_command_str[1024]" not in region
assert "char new_funcargs[1024]" not in region
assert "snprintf(NULL, 0, \"<%s>\", tree->name)" in region
assert "XtFree(new_command_str);" in region
assert "XtFree(new_funcargs);" in region
print("mwm inline command-tree names are dynamically bounded")
