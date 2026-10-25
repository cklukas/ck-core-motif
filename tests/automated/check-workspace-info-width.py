#!/usr/bin/env python3
"""Check that Mwm workspace-info numeric slots fit native unsigned longs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmProperty.c").read_text(
    encoding="utf-8"
)
assert "#define WIP_NUMBER_SIZE" in source
assert "(2 * sizeof(unsigned long)) + 1" in source
assert source.count("snprintf (&pch[ix], WIP_NUMBER_SIZE") >= 6
assert "sprintf (&pch[ix]" not in source
print("Mwm workspace-info numeric fields use native-width bounded slots")
