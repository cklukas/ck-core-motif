#!/usr/bin/env python3
"""Check MWM numeric resource arguments use checked conversion helpers."""
from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResParse.c").read_text(
    encoding="utf-8"
)

parse_start = source.index("static Boolean ParseWmFuncNbrArg")
parse_end = source.index("} /* END OF FUNCTION ParseWmFuncNbrArg */", parse_start)
parse = source[parse_start:parse_end]
assert "string = GetString (linePP);" in parse
assert "string == NULL || !StrToNum (string, &val)" in parse
assert "*pNumber = (unsigned long)val;" in parse

for name, base in (("StrToNum", "10U"), ("StrToHex", "16U"), ("StrToOct", "8U")):
    signature = f"static Boolean {name}(unsigned char *str"
    start = source.rindex(signature)
    end = source.index("} /* END OF FUNCTION", start)
    region = source[start:end]
    assert "str == NULL || value == NULL" in region
    assert f"(UINT_MAX - digit) / {base}" in region
    assert "return (-1)" not in region

print("PASS MWM numeric arguments reject null, invalid, and overflowing values")
