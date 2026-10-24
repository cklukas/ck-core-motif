#!/usr/bin/env python3
"""Check Mrm widget-reference name and capacity arithmetic."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwrefs.c"
text = source.read_text(encoding="utf-8")
start = text.index("Urm__CW_AddWRef (")
end = text.index("\n}\n", start)
region = text[start:end]
for needle in (
    "name_len = strlen(w_name);",
    "name_len >= MrmMaxResourceSize",
    "name_bytes > SIZE_MAX - sizeof(URMWRef)",
    "bytes_used > UrmRCBufSize(wref_id)",
    "bytes_needed > UrmRCBufSize(wref_id) - bytes_used",
):
    if needle not in region:
        raise SystemExit(f"Mrm widget-reference bound is missing: {needle}")
if "MrmCount\t\tname_bytes" in region:
    raise SystemExit("widget-reference name length still narrows before validation")

print("Mrm widget-reference storage sizing is bounded")
