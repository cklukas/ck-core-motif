#!/usr/bin/env python3
"""Check Mrm writer scalar setters reject null structured values."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrwr.c"
text = source.read_text(encoding="utf-8")
start = text.index("UrmCWRSetArgValue (")
end = text.index("\n\n\f", start)
region = text[start:end]

for check in ("if (arg_val == 0)", "if (src_ct == NULL)"):
    if check not in region:
        raise SystemExit(f"scalar writer lacks null guard: {check}")

float_pos = region.index("case MrmRtypeFloat:")
float_guard = region.index("if (arg_val == 0)", float_pos)
float_deref = region.index("*((double *) arg_val)", float_guard)
if float_guard > float_deref:
    raise SystemExit("float value is dereferenced before its null guard")

color_pos = region.index("case MrmRtypeColorTable:")
color_guard = region.index("if (src_ct == NULL)", color_pos)
color_use = region.index("UrmColorTableSize(src_ct)", color_guard)
if color_guard > color_use:
    raise SystemExit("color table is sized before its null guard")

print("Mrm writer scalar values validate null inputs")
