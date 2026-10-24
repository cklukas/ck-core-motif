#!/usr/bin/env python3
"""Check legacy Mrm callback descriptor counts before allocation."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"
text = source.read_text(encoding="utf-8")
start = text.index("Urm__CW_TranslateOldCallback (")
end = text.index("\n}\n", start)
region = text[start:end]
for needle in (
    "oldptr == NULL",
    "oldptr->count < 0",
    "(size_t)oldptr->count",
    "SIZE_MAX - sizeof(RGMCallbackDesc)",
    "sizeof(RGMCallbackItem)",
    "return NULL",
):
    if needle not in region:
        raise SystemExit(f"Mrm callback descriptor guard missing: {needle}")

print("Mrm legacy callback descriptor counts are bounded")
