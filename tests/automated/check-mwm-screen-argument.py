#!/usr/bin/env python3
"""Check that MWM rejects malformed f.screen arguments."""
from pathlib import Path

source = (Path(__file__).resolve().parents[2] / "clients" / "mwm" / "WmFunction.c").read_text()
start = source.index("Boolean F_Screen")
end = source.index("\n}\n\n\n\f", start)
region = source[start:end]

assert "parsed = strtol(args, &end, 10);" in region
assert "errno == ERANGE" in region
assert "parsed < INT_MIN || parsed > INT_MAX" in region
assert "*end != '\\0'" in region
assert 'Warning("Invalid screen number");' in region
assert "scr = atoi (args);" not in region
print("MWM screen argument contract passed")
