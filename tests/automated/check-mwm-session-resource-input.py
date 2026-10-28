#!/usr/bin/env python3
"""Check that MWM session-resource integers are parsed with bounds checks."""
from pathlib import Path

source = (Path(__file__).resolve().parents[2] / "clients" / "mwm" / "WmXSMP.c").read_text()
assert "parse_session_int(const char *text, int *value)" in source
start = source.index("parse_session_int(const char *text, int *value)")
end = source.index("\n}\n", start) + 3
helper = source[start:end]
assert "parsed = strtol(text, &end, 10);" in helper
assert "errno == ERANGE" in helper
assert "parsed < INT_MIN || parsed > INT_MAX" in helper
assert "*end != '\\0'" in helper
assert "atoi(" not in source
assert source.count("parse_session_int(resourcePtr") >= 16
print("MWM session-resource input contract passed")
