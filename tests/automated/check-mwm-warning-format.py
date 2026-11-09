#!/usr/bin/env python3
"""Verify MWM treats catalog warnings as data, not variadic printf formats."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmMenu.c").read_text(
    encoding="utf-8"
)
start = source.index("void MWarning")
end = source.index("} /* END OF FUNCTION MWarning */", start)
body = source[start:end]

assert "snprintf(pch, sizeof pch, format, message)" not in body
assert "const char *input = message ? message : \"\";" in body
assert "*cursor == '%' && cursor[1] == 's'" in body
assert "*cursor == '%' && cursor[1] == '%'" in body
assert "while (*value && output < limit)" in body
assert "*output = '\\0';" in body

print("PASS MWM warning formatter bounds catalog substitutions")
