#!/usr/bin/env python3
"""Check that XmRenderT token scanning rejects int-position overflow."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c"
text = source.read_text(encoding="utf-8")
start = text.index("ReadToken(char *string")
end = text.index("\nstatic", start)
region = text[start:end]

required = (
    "if (pos == INT_MAX)",
    "if (count == INT_MAX)",
    "end_offset > INT_MAX - pos",
)
missing = [item for item in required if item not in region]
if missing:
    raise SystemExit("render-token overflow guards missing: " + ", ".join(missing))

print("render-token position and length overflow guards present")
