#!/usr/bin/env python3
"""Check that widget-reference lookup validates inputs and stored names."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwrefs.c").read_text(encoding="utf-8")
start = source.index("Urm__CW_FindWRef (URMResourceContextPtr")
end = source.index("\n}\n", start)
body = source[start:end]

for guard in (
    "if (w_id_return == NULL)",
    "*w_id_return = NULL;",
    "if (!UrmRCValid(wref_id) || w_name == NULL)",
    "refdsc->num_refs < 0 || refdsc->heap_size < 0",
    "header_size > buffer_size",
    "w_name_offs < heap_start",
    "strnlen(UrmWRefNameN(refdsc,ndx), name_size) == name_size",
):
    if guard not in body:
        raise SystemExit(f"Mrm widget-reference lookup guard is missing: {guard}")

if body.index("*w_id_return = NULL;") > body.index("refdsc = (URMWRefStructPtr) UrmRCBuffer"):
    raise SystemExit("Mrm widget-reference lookup clears its result too late")

print("Mrm widget-reference lookup validates inputs and stored name bounds")
