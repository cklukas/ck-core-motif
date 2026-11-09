#!/usr/bin/env python3
"""Check Panelist function-argument parsing bounds its table lookup."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResParse.c").read_text(
    encoding="utf-8"
)
start = source.index("ParseWmFunctionArg (\n")
end = source.index("} /* END OF FUNCTION ParseWmFunctionArg */", start)
region = source[start:end]

assert "if (ppArg != NULL)\n\t*ppArg = NULL;" in region
assert "if (linePP == NULL || ppArg == NULL || ix < 0 ||" in region
assert "(size_t)ix >= WMFUNCTIONTABLESIZE" in region
assert "functionTable[ix].parseProc == NULL" in region
assert region.index("(size_t)ix >= WMFUNCTIONTABLESIZE") < region.index(
    "functionTable[ix].parseProc == ParseWmFuncStrArg"
)

print("PASS MWM Panelist function arguments validate table indexes")
