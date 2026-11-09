#!/usr/bin/env python3
"""Check f.exec delimiter handling is independent of final character width."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResParse.c").read_text(
    encoding="utf-8"
)
start = source.index("Boolean ParseWmFuncStrArg (")
end = source.index("} /* END OF FUNCTION ParseWmFuncStrArg */", start)
region = source[start:end]

assert "append_delimiter = (len == 0);" in region
assert "append_delimiter = (last != L'&');" in region
assert "if (append_delimiter)" in region
assert "(*pArgs)[len] = '&';" in region
assert "(*pArgs)[len + 1] = '\\0';" in region
assert "if (lastlen == 1 && last != wdelim)" not in region

print("PASS MWM f.exec delimiter supports multibyte command endings")
