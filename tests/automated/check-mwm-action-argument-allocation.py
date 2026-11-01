#!/usr/bin/env python3
"""Check MWM action argument growth fits both size_t and Xt Cardinal."""
from pathlib import Path


source = (Path(__file__).parents[2] / "clients/mwm/WmResParse.c").read_text(
    encoding="utf-8"
)
start = source.index("Boolean ParseWmFuncActionArg")
end = source.index("} /* END OF FUNCTION ParseWmFuncActionArg */", start)
region = source[start:end]
guard = "(size_t)new_arg_size > UINT_MAX / sizeof(DtActionArg)"

assert "(size_t)new_arg_size > SIZE_MAX / sizeof(DtActionArg)" in region
assert guard in region
assert region.index(guard) < region.index("XtRealloc((char *)pAP->aap")
print("PASS MWM action argument growth fits Xt Cardinal allocations")
