#!/usr/bin/env python3
"""Check Mrm interface-module topmost index validation."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmmodule.c"
text = source.read_text(encoding="utf-8")
start = text.index("UrmIFMSetTopmost (")
end = text.index("\n\n\f", start)
region = text[start:end]

for check in ("if ( index == NULL )", "index_len = strlen(index)",
              "if ( index_len > URMMaxIndexLen )"):
    if check not in region:
        raise SystemExit(f"topmost index validation missing: {check}")
if "strncat" in region:
    raise SystemExit("topmost index still uses append semantics")
if "memcpy (ifmodptr->topmost[topmost_ndx].index, index, index_len + 1)" not in region:
    raise SystemExit("topmost index is not copied with its validated length")

print("Mrm topmost indices are validated and replaced safely")
