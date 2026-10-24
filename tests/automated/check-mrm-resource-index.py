#!/usr/bin/env python3
"""Check indexed Mrm resources reject null keys before strlen."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrwr.c"
text = source.read_text(encoding="utf-8")
start = text.rindex("UrmCWR__AppendResource (")
end = text.index("\n}\n", start)
region = text[start:end]
null_pos = region.index("if (index == NULL)")
strlen_pos = region.index("strlen(index)")
if null_pos > strlen_pos:
    raise SystemExit("Mrm resource index is dereferenced before its null guard")
if "MrmNULL_INDEX" not in region[null_pos:strlen_pos]:
    raise SystemExit("Mrm resource null-index failure is missing")

print("Mrm indexed resource keys validate null input")
