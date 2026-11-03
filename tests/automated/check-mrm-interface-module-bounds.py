#!/usr/bin/env python3
"""Check interface-module fetches bound the on-record topmost count."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmappl.c").read_text(
    encoding="utf-8"
)
start = source.index("MrmFetchInterfaceModule (MrmHierarchy")
end = source.index("MrmFetchWidget (MrmHierarchy", start)
body = source[start:end]

validation = body.index("if ( ! UrmInterfaceModuleValid(modptr) ||")
loop = body.index("for ( ndx=0 ; ndx<modptr->count ; ndx++ )")
assert validation < loop
for needle in (
    "UrmRCSize(mod_context) < sizeof(RGMModuleDesc)",
    "modptr->count < 0",
    "(size_t)(modptr->count - 1)",
    "sizeof(RGMTopmostDesc)",
):
    assert needle in body[validation:loop], needle
assert "MrmBAD_IF_MODULE" in body[validation:loop]

print("Mrm interface-module fetch bounds its topmost-widget descriptors")
