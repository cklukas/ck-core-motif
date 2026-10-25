#!/usr/bin/env python3
"""Check MWM workspace resources duplicate icon geometry only once."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResource.c").read_text(
    encoding="utf-8"
)
start = source.index("void \nProcessWorkspaceResources")
body = source[start : source.index("} /* END OF FUNCTION ProcessWorkspaceResources", start)]
assert body.count("pWS->iconBoxGeometry = XtNewString") == 1
assert "Dup iconbox geometry" in body

print("MWM workspace icon-box geometry is duplicated once")
