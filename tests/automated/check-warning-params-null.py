#!/usr/bin/env python3
"""Check that Motif warning formatting tolerates a NULL parameter array."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "VendorS.c"
text = source.read_text(encoding="utf-8")
start = text.index("MotifWarningHandler (String name")
end = text.index("\n\n\n/************************************************************************", start)
region = text[start:end]

required = "if (params && num_params && *num_params > 1)"
if region.count(required) != 2:
    raise SystemExit("warning formatter NULL-params guards missing")

print("warning formatter handles NULL parameter arrays")
