#!/usr/bin/env python3
"""Check DataField drawing rejects invalid wide conversion results."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataF.c").read_text()
start = source.index("df_DrawText(\n        XmDataFieldWidget")
end = source.index("static int\n", start)
body = source[start:end]

assert body.count("size_t num_bytes = 0;") == 2
assert body.count("num_bytes = wcstombs") == 2
assert body.count("num_bytes != (size_t)-1") == 2
assert "num_bytes <= (size_t)INT_MAX" in body
measure_start = source.index("df_FindPixelLength(\n        XmDataFieldWidget")
measure = source[measure_start:source.index("static void\n", measure_start)]
assert measure.count("num_bytes = wcstombs") == 2
assert "num_bytes != (size_t)-1" in measure

print("PASS DataField drawing conversion rejects invalid input")
