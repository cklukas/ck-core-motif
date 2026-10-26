#!/usr/bin/env python3
"""Check MWM per-screen atom arrays are bounded before Xt allocation."""
from pathlib import Path


source = (Path(__file__).parents[2] / "clients/mwm/WmProtocol.c").read_text(
    encoding="utf-8"
)
assert source.count("wmGD.numScreens < 0") >= 2
assert source.count("(size_t)wmGD.numScreens > SIZE_MAX / sizeof(Atom)") >= 2
assert source.count("(size_t)wmGD.numScreens > (size_t)UINT_MAX / sizeof(Atom)") >= 2
assert "(Cardinal)((size_t)wmGD.numScreens * sizeof(Atom))" in source
assert "atom_bytes = (size_t)wmGD.numScreens * sizeof(Atom);" in source
assert "XtMalloc (wmGD.numScreens * (sizeof (Atom)))" not in source
print("PASS MWM per-screen atom allocations are bounded")
