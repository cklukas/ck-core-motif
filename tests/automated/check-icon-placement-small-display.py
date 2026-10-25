#!/usr/bin/env python3
"""Check that Mwm icon placement handles one-slot displays safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmIPlace.c").read_text(
    encoding="utf-8"
)
assert "if (pWS->IPData.placementCols < 1)" in source
assert "if (pWS->IPData.placementRows < 1)" in source
assert "placementCols > 1" in source
assert "placementRows > 1" in source
assert "while (iSpaceX < MINIMUM_ICON_SPACING);" not in source
assert "while (iSpaceY < MINIMUM_ICON_SPACING);" not in source
print("Mwm icon placement avoids zero-divisor spacing cases")
