#!/usr/bin/env python3
"""Check primitive pointer-backed Mrm conversions reject null values."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"
text = source.read_text(encoding="utf-8")
start = text.index("\nUrm__CW_ConvertValue (")
end = text.find("\n\n\f", start)
region = text[start:end]
for needle in (
    "if ( colorptr == NULL )",
    "if ( wcharentry == NULL )",
    "case MrmRtypeTransTable:\n      if ( *val == 0 )",
    "case MrmRtypeClassRecName:\n      if ( *val == 0 )",
    "case MrmRtypeKeysym:\n      if ( *val == 0 )",
    "if ( unitsintvalue == NULL )",
    "if ( unitsfloatvalue == NULL )",
):
    if needle not in region:
        raise SystemExit(f"missing conversion guard: {needle}")
print("PASS Mrm primitive pointer conversions reject null values")
