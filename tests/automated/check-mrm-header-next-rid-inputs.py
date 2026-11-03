#!/usr/bin/env python3
"""Check that Mrm header RID allocation validates its input and output pointers."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIheader.c"
text = source.read_text(encoding="utf-8")
start = text.index("\nIdb__HDR_NextRID (")
end = text.find("\n", start + 1)
region = text[start:] if end < 0 else text[start:end]
output_guard = region.index("if (res_id_return == NULL)")
clear_output = region.index("*res_id_return = 0;", output_guard)
file_guard = region.index("if (file_id == NULL)", clear_output)
state_read = region.index("resndx =", file_guard)
if not output_guard < clear_output < file_guard < state_read:
    raise SystemExit("Mrm header RID allocation does not clear output before failure paths")
print("Mrm header RID allocation validates inputs and clears output")
