#!/usr/bin/env python3
"""Check Mrm compound-string byte-stream ownership is released."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrwr.c"
text = source.read_text(encoding="utf-8")
start = text.index("Cardinal \nUrmCWR__AppendCString (")
end = text.index("\n\n\f", start)
region = text[start:end]

if region.count("XtFree ((char *)asn1)") < 3:
    raise SystemExit("compound-string byte stream is not released on every path")
copy_pos = region.index("memcpy (cstgadr, (char *)asn1, len)")
free_pos = region.index("XtFree ((char *)asn1);", copy_pos)
if free_pos < copy_pos:
    raise SystemExit("compound-string stream is freed before the record copy")

print("Mrm compound-string streams are released after copying")
