#!/usr/bin/env python3
"""Check compound-string rendition-tag copies roll back allocations."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "XmStringSeg.c"
text = source.read_text()
start = text.index("static Boolean\nCopyRenditionTags")
end = text.index("/*\n * _XmStringGetSegment", start)
helper = text[start:end]
if "if (tags == NULL)" not in helper:
    raise SystemExit("rendition array allocation is not guarded")
if "XtFree((char *)tags[--i]);" not in helper:
    raise SystemExit("partial rendition copies are not released")
if "copy_data ? XtNewString" not in helper:
    raise SystemExit("rendition tags are not copied through the guarded helper")

segment_start = text.index("Boolean\n_XmStringGetSegment")
segment_end = text.index("Boolean \n_XmStringGetNextSegment", segment_start)
segment = text[segment_start:segment_end]
if segment.count("CopyRenditionTags(") < 2:
    raise SystemExit("both rendition paths do not use the guarded helper")

print("compound-string rendition copies guard allocation failures")
