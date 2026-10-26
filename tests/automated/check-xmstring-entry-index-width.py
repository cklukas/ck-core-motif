#!/usr/bin/env python3
"""Check that XmString entry copying preserves the segment-count width."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c"
text = source.read_text(encoding="utf-8")
start = text.index("_XmStringEntryCopy(_XmStringEntry entry)", 100000)
end = text.index("/** Begin macros", start)
region = text[start:end]

required = (
    "size_t segment_count",
    "size_t segment_index",
    "segment_index < segment_count",
    "for (j = 0; j < segment_index; ++j)",
)
missing = [item for item in required if item not in region]
if missing:
    raise SystemExit("XmString entry index width guard missing: " + ", ".join(missing))
if "(int)segment_count" in region:
    raise SystemExit("XmString entry copy still narrows segment_count")

print("XmString entry copying preserves segment-count width")
