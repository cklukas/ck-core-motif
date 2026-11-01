#!/usr/bin/env python3
"""Check MWM initializes every per-screen selection-atom slot."""
from pathlib import Path


source = (Path(__file__).parents[2] / "clients/mwm/WmProtocol.c").read_text(
    encoding="utf-8"
)
start = source.index("static void\nOwnWMSelections")
end = source.index("\n\n\n\n\f", start)
region = source[start:end]
assert "XtCalloc((Cardinal)wmGD.numScreens, sizeof(Atom))" in region
assert "XtMalloc((Cardinal)atom_bytes)" not in region
assert "Initialize unmanaged-screen entries to None" in region
print("PASS MWM per-screen selection atoms start as None")
