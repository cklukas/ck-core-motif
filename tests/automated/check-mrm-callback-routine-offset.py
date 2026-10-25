#!/usr/bin/env python3
"""Check callback fixup rejects empty routine-name offsets."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"
text = source.read_text(encoding="utf-8")
start = text.rfind("\nUrm__CW_FixupCallback (")
end = text.find("\n\n\f", start)
region = text[start:end]
needle = "itmptr->cb_item.routine < RGMWidgetRecordHdrSize"
if needle not in region or "memchr ((char *)bufptr + itmptr->cb_item.routine" not in region:
    raise SystemExit("callback routine offset guard is missing")
print("PASS Mrm callback fixup rejects empty routine offsets")
