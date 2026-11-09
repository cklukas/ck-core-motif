#!/usr/bin/env python3
"""Check the exported MWM function parser validates caller pointers."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResParse.c").read_text(
    encoding="utf-8"
)
start = source.rindex("int ParseWmFunction (")
end = source.index("} /* END OF FUNCTION ParseWmFunction */", start)
region = source[start:end]

assert "if (pWmFunction == NULL)" in region
assert "if (linePP == NULL || *linePP == NULL)" in region
assert "*pWmFunction = F_Nop;" in region
assert region.index("if (linePP == NULL || *linePP == NULL)") < region.index(
    "lineP = *linePP;"
)
assert region.count("return (F_NOP_INDEX);") >= 3

print("PASS exported MWM function parser validates caller pointers")
