#!/usr/bin/env python3
"""Check MWM numeric KeySym parsing uses the checked converter correctly."""
from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResParse.c").read_text(
    encoding="utf-8"
)
start = source.index("static Boolean ParseKeySym")
end = source.index("} /* END OF FUNCTION ParseKeySym */", start)
parse = source[start:end]

assert "unsigned int   numericKeySym;" in parse
assert "!isdigit ((unsigned char)keySymName[0])" in parse
assert "!StrToNum ((unsigned char *)keySymName, &numericKeySym)" in parse
assert "*detail = numericKeySym;" in parse
assert "StrToNum ((unsigned char *)&keySymName[0])" not in parse
assert "== -1" not in parse

print("PASS MWM numeric KeySym parsing uses checked conversion")
