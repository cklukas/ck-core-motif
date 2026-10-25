#!/usr/bin/env python3
"""Check MWM TARGETS conversion handles atom-array allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmProtocol.c").read_text()
start = source.index("WMiConvert (")
end = source.index("\n#ifdef MWM_WSM", start)
region = source[start:end]

assert "Atom *targs" in region
assert "if (targs == NULL)" in region
assert "found = False;" in region
assert region.index("if (targs == NULL)") < region.index("*output = (XtPointer) targs")
print("MWM TARGETS conversion handles allocation failure")
