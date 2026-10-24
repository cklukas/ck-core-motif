#!/usr/bin/env python3
"""Check that Mrm index resource enumeration propagates list append failures."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIindex.c"
text = source.read_text(encoding="utf-8")
start = text.index("\nIdb__INX_FindResources (")
end = text.find("\n", start + 1)
region = text[start:] if end < 0 else text[start:end]
if region.count("result = UrmPlistAppendString") < 2:
    raise SystemExit("Mrm resource enumeration does not capture both append results")
if region.count("if (result != MrmSUCCESS) return result;") < 2:
    raise SystemExit("Mrm resource enumeration does not propagate append failures")
print("Mrm resource enumeration propagates pointer-list append failures")
