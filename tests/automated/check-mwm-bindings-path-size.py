#!/usr/bin/env python3
"""Check MWM motif-bindings path construction rejects size overflow."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResParse.c").read_text(
    encoding="utf-8"
)
start = source.rindex("void ProcessMotifBindings")
body = source[start : source.index("} /* END OF FUNCTION ProcessMotifBindings", start)]
assert "homeLength;" in body
assert "bindingLength;" in body
assert "homeLength > SIZE_MAX - bindingLength - 2" in body
assert "homeLength + bindingLength + 2 > UINT_MAX" in body
assert "fileNameLength = homeLength + bindingLength + 2;" in body
assert "XtMalloc((Cardinal)fileNameLength)" in body
assert "XtMalloc((unsigned int)fileNameLength)" not in body

print("MWM motif-bindings path sizing is overflow checked")
