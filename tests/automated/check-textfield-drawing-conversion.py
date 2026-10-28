#!/usr/bin/env python3
"""Check TextField drawing rejects invalid wide-character conversions."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text()
start = source.index("static void\nDrawText(")
end = source.index("static int\nFindPixelLength", start)
draw = source[start:end]

assert draw.count("size_t num_bytes = 0;") == 2
assert draw.count("num_bytes = wcstombs") == 2
assert draw.count("num_bytes != (size_t)-1") == 2
assert "(int)num_bytes" in draw
assert "(int)((length + 1) * sizeof(wchar_t))" not in draw

measure_start = source.index("static int\nFindPixelLength(")
measure = source[measure_start:source.index("static void\nDrawTextSegment", measure_start)]
assert measure.count("size_t num_bytes") == 2
assert measure.count("num_bytes = wcstombs") == 2
assert "num_bytes == (size_t)-1" in measure
assert "(int)((length + 1)*sizeof(wchar_t))" not in measure

print("PASS TextField drawing conversion guards")
