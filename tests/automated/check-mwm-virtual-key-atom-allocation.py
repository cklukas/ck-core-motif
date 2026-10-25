#!/usr/bin/env python3
"""Check MWM virtual-key remapping handles atom-array allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmInitWs.c").read_text(
    encoding="utf-8"
)
start = source.index("Boolean\nVirtKeys4DIN")
end = source.index("#endif /* NO_HP_KEY_REMAP */", start)
region = source[start:end]

allocation = "prop_atoms = (Atom *) XtMalloc( sizeof( Atom) * num_props) ;"
assert allocation in region
check = "if (prop_atoms == NULL)"
assert check in region
assert region.index(check) > region.index(allocation)
failure = region[region.index(check):region.index("XInternAtoms", region.index(check))]
assert "XtFree( (char *) vkeysym_xref) ;" in failure
assert "return PropChanged4DIN ;" in failure
print("PASS MWM virtual-key atom allocation failure is handled")
