#!/usr/bin/env python3
"""Check VendorShell MWM menu copies are transactional."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "VendorS.c").read_text()
start = source.index("SetMwmStuff(")
start = source.index("static void", start)
end = source.index("\n/* The AddGrab", start)
region = source[start:end]

assert "char *\t\treplacement;" in region
assert "replacement = nve->vendor.mwm_menu" in region
assert "if (!nve->vendor.mwm_menu || replacement != NULL)" in region
assert "nve->vendor.mwm_menu = ove->vendor.mwm_menu;" in region
assert "nve->vendor.mwm_menu = NULL;" in region
assert region.index("XtNewString(nve->vendor.mwm_menu)") < region.index("XtFree(ove->vendor.mwm_menu)")
print("VendorShell MWM menu preserves state on allocation failure")
