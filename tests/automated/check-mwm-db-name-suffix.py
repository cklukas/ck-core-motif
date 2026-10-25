#!/usr/bin/env python3
"""Ensure MWM session database suffix updates avoid overflowing conversions."""

from pathlib import Path


text = (Path(__file__).parents[2] / "clients" / "mwm" / "WmXSMP.c").read_text(
    encoding="utf-8"
)
start = text.rindex("setClientDBName(void)")
end = text.index("static char **", start)
function = text[start:end]
assert "strtoul(ptr, &endptr, 10)" in function
assert "errno != ERANGE" in function
assert "numSuffix != ULONG_MAX" in function
assert "snprintf" in function
assert "strcat(wmGD.dbFileName, \".0\")" not in function
print("MWM session database suffix updates are bounded")
