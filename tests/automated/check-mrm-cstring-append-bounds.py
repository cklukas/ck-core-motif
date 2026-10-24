#!/usr/bin/env python3
"""Check compound-string byte lengths before Mrm record allocation."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrwr.c"
text = source.read_text(encoding="utf-8")
start = text.rindex("UrmCWR__AppendCString (")
end = text.index("\n}\n", start)
region = text[start:end]
for needle in (
    "size_t",
    "encoded_len = XmCvtXmStringToByteStream",
    "encoded_len <= 0",
    "(size_t)encoded_len > MrmMaxResourceSize",
    "len = (size_t)encoded_len",
):
    if needle not in region:
        raise SystemExit(f"Mrm compound-string append guard missing: {needle}")

print("Mrm compound-string append sizing is bounded")
