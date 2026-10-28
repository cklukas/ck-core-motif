#!/usr/bin/env python3
"""Check MWM session-hint counts are validated before allocation."""

from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
SOURCE = (ROOT / "clients/mwm/WmResParse.c").read_text(encoding="utf-8")
START = SOURCE.index("Boolean GetSessionHintsInfo")
END = SOURCE.index("} /* END OF FUNCTION  GetSessionHintsInfo", START)
FUNCTION = SOURCE[START:END]

assert "pSD == NULL || numItems <= 0" in FUNCTION
assert "UINT_MAX / sizeof (DtSessionItem)" in FUNCTION
assert "SIZE_MAX / sizeof (DtSessionItem)" in FUNCTION
assert "allocation_bytes = (size_t)numItems * sizeof (DtSessionItem)" in FUNCTION
assert "XtMalloc ((Cardinal)allocation_bytes)" in FUNCTION
assert "memset ((char *)pSD->pDtSessionItems, 0, allocation_bytes)" in FUNCTION
assert "XtMalloc (numItems * sizeof (DtSessionItem))" not in FUNCTION
assert "memset ((char *)pSD->pDtSessionItems, NULL" not in FUNCTION

print("MWM session-hint allocation guards present")
