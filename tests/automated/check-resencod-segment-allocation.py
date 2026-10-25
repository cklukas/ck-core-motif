#!/usr/bin/env python3
"""Check extended compound-text segment buffers handle allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ResEncod.c").read_text()
first_start = source.index("processExtendedSegmentsHack(\n", 10000)
second_start = source.index("processExtendedSegments(\n", 50000)
first = source[first_start:source.index("processESC(\n", first_start)]
second = source[second_start:source.index("process94n(\n", second_start)]
for body in (first, second):
    assert body.count("if (charset_copy == NULL)") == 1
    assert body.count("if (text_copy == NULL)") == 1
    assert "XtFree(charset_copy);" in body
print("PASS extended compound-text segment buffers handle allocation failure")
