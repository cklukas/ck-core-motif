#!/usr/bin/env python3
"""Check that float-unit conversion validates both output pointers."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ResInd.c").read_text(
    encoding="utf-8"
)
start = source.index("_XmConvertFloatUnitsToIntUnits(int unitType")
end = source.index("/**********************************************************************", start + 1)
region = source[start:end]
assert "if (intUnitType == NULL || intUnitValue == NULL)" in region
assert "*intUnitType =" in region
assert "*intUnitValue =" in region
print("PASS float-unit conversion validates output pointers")
