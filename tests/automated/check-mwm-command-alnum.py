#!/usr/bin/env python3
"""Check that MWM command parsing classifies bytes safely with isalnum."""
from pathlib import Path

source = (Path(__file__).resolve().parents[2] / "clients" / "mwm" / "WmResParse.c").read_text()
start = source.rindex("static Boolean ParseClientCommand")
end = source.index("\n}\n", start)
region = source[start:end]

assert "isalnum((unsigned char)*stream)" in region
assert "isalnum(*stream)" not in region
print("MWM command alnum contract passed")
