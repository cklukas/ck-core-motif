#!/usr/bin/env python3
"""Check that MWM menu matching classifies bytes safely with isspace."""
from pathlib import Path

source = (Path(__file__).resolve().parents[2] / "clients" / "mwm" / "WmMenu.c").read_text()
start = source.index("PerformClientCommandMatch (clientCommand")
end = source.index("\n}\n", start)
region = source[start:end]

assert region.count("isspace((unsigned char)") == 7
assert "isspace(*menuItemCommand)" not in region
assert "isspace(*clientCommand)" not in region
assert "isspace(*bestMatchStr)" not in region
print("MWM menu whitespace contract passed")
