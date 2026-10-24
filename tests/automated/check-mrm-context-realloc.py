#!/usr/bin/env python3
"""Check Mrm resource-context resizing preserves ownership on realloc failure."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmcontext.c"
text = source.read_text(encoding="utf-8")
start = text.index("UrmResizeResourceContext (")
end = text.index("\n\n\f", start)
region = text[start:end]

if "newbuf = XtRealloc (context_id->data_buffer, size)" not in region:
    raise SystemExit("resource-context resize does not retain realloc result")
if "if ( newbuf == NULL )" not in region:
    raise SystemExit("resource-context realloc has no failure guard")
if region.index("if ( newbuf == NULL )") < region.index(
    "newbuf = XtRealloc (context_id->data_buffer, size)"
):
    raise SystemExit("realloc failure guard precedes the realloc")
if region.index("context_id->data_buffer = newbuf") < region.index(
    "if ( newbuf == NULL )"
):
    raise SystemExit("resource buffer is published before the realloc guard")
if "MrmFAILURE" not in region:
    raise SystemExit("realloc failure does not return MrmFAILURE")

print("Mrm resource-context reallocations preserve ownership")
