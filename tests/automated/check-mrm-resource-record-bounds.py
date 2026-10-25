#!/usr/bin/env python3
"""Check embedded Mrm resource descriptors before field access."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"
text = source.read_text(encoding="utf-8")
for needle in (
    "static Boolean\nUrmResourceRecordValid",
    "offsetof(RGMResourceDesc, key)",
    "resptr->size < key_offset + 1 || resptr->size > available",
    "resptr->type == URMrRID && resptr->size < sizeof(RGMResourceDesc)",
    "memchr((char *)resptr + key_offset",
):
    if needle not in text:
        raise SystemExit(f"missing resource-record bound: {needle}")
if text.count("UrmResourceRecordValid(") < 3:
    raise SystemExit("resource descriptor validator is not used by both paths")
print("PASS Mrm embedded resource descriptors are size- and string-bounded")
