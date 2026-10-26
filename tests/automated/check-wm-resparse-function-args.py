#!/usr/bin/env python3
"""Check MWM function-argument length and empty-string handling."""
from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResParse.c").read_text(
    encoding="utf-8"
)

maybe_start = source.index("Boolean ParseWmFuncMaybeStrArg")
maybe_end = source.index("} /* END OF FUNCTION ParseWmFuncMaybeStrArg */", maybe_start)
maybe = source[maybe_start:maybe_end]
assert "size_t len;" in maybe
assert "if (len >= UINT_MAX)" in maybe
assert "XtMalloc ((Cardinal)(len + 1))" in maybe

string_start = source.index("Boolean ParseWmFuncStrArg")
string_end = source.index("} /* END OF FUNCTION ParseWmFuncStrArg */", string_start)
string = source[string_start:string_end]
assert "size_t  len;" in string
assert "len > SIZE_MAX - 2 || len + 2 > UINT_MAX" in string
assert "XtMalloc ((Cardinal)(len + 2))" in string
assert "((lastlen = mblen(p, MB_CUR_MAX)) > 0)" in string
assert "len == 0 || (*pArgs)[len - 1] != '&'" in string
print("PASS MWM function-argument lengths and empty F_Exec are guarded")
