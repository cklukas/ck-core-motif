#!/usr/bin/env python3
"""Check Mrm callback cleanup tolerates null and truncated ownership lists."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"
text = source.read_text(encoding="utf-8")
start = text.index("UrmDestroyCallback (Widget")
end = text.find("\n\n\f", start)
region = text[start:] if end < 0 else text[start:end]
guard = region.find("if (list_id == NULL || list_id->ptr_vec == NULL ||")
if guard < 0:
    raise SystemExit("callback cleanup lacks a null-list guard")
loop = region.find("for (ndx=0 ; ndx + 1 < list_id->num_ptrs ; ndx++)", guard)
if loop < 0:
    raise SystemExit("callback cleanup can iterate past a missing value pair")
trailing = region.find("if (ndx < list_id->num_ptrs)", loop)
if trailing < 0 or trailing - loop > 2600:
    raise SystemExit("callback cleanup does not discard a dangling final type tag")
free = region.find("UrmPlistFree (list_id)", loop)
if free < 0:
    raise SystemExit("callback cleanup no longer releases the ownership list")

print("Mrm callback cleanup handles null and truncated ownership lists")
