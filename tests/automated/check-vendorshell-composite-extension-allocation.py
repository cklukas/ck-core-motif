#!/usr/bin/env python3
"""Check VendorShell inherited composite-extension allocation guards."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "VendorS.c").read_text()
start = source.index("compExtPtr = (CompositeClassExtensionRec **) ")
end = source.index("\n    /* Do this here because of bug in Xt */", start)
region = source[start:end]

for needle in (
    "if (compExtPtr == NULL)",
    "if (superExtPtr == NULL || *superExtPtr == NULL)",
    "*compExtPtr = XtNew(CompositeClassExtensionRec);",
    "if (*compExtPtr == NULL)",
):
    assert needle in region, needle
assert region.index("if (*compExtPtr == NULL)") < region.index("memcpy((char*)*compExtPtr")

print("PASS VendorShell composite-extension allocation guards present")
