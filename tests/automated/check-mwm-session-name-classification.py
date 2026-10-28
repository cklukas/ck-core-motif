#!/usr/bin/env python3
"""Check that MWM session-name digit classification is defined for all bytes."""
from pathlib import Path

source = (Path(__file__).resolve().parents[2] / "clients" / "mwm" / "WmXSMP.c").read_text()
start = source.index("static void\nsetClientDBName(void)")
end = source.index("\n}\n", start)
region = source[start:end]

assert "isdigit((unsigned char)*p1)" in region
assert "isdigit(*p1)" not in region
print("MWM session-name classification contract passed")
