#!/usr/bin/env python3
"""Check SetValues callback descriptor copies before allocation."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwrefs.c"
text = source.read_text(encoding="utf-8")
start = text.index("Urm__CW_AppendCBSVWidgetRef (")
end = text.index("\n}\n", start)
region = text[start:end]
for needle in (
    "cbptr == NULL",
    "cbptr->count < 0",
    "(size_t)cbptr->count",
    "SIZE_MAX - sizeof(RGMCallbackDesc)",
    "(size_t)UINT_MAX - sizeof(RGMCallbackDesc)",
    "sizeof(RGMCallbackItem)",
    "size_t\t\tdescsize",
):
    if needle not in region:
        raise SystemExit(f"Mrm callback-copy guard missing: {needle}")

print("Mrm callback descriptor copies are bounded")
