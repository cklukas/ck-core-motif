#!/usr/bin/env python3
"""Check MWM group parsing clamps pointer distances in size_t."""
from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResParse.c").read_text(
    encoding="utf-8"
)
start = source.index("static Boolean ParseWmFuncGrpArg")
end = source.index("} /* END OF FUNCTION ParseWmFuncGrpArg */", start)
region = source[start:end]
assert "size_t len;" in region
assert "len = (size_t)(lineP - startP);" in region
assert "if (len > MAX_GROUP_STRLEN)" in region
assert "len = min (lineP - startP, MAX_GROUP_STRLEN)" not in region
print("PASS MWM group parsing uses bounded size_t lengths")
