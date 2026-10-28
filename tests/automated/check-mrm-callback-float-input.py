#!/usr/bin/env python3
"""Ensure callback float values are validated before record mutation."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrwr.c").read_text(
    encoding="utf-8"
)
start = text.index("UrmCWRSetCallbackItem (")
body = text[start:text.index("\n}\n", start)]
guard = "if ((type == MrmRtypeFloat ||"
assert guard in body
guard_pos = body.index(guard)
append_pos = body.index("UrmCWR__AppendString", guard_pos)
assert "itm_val == 0" in body[guard_pos:append_pos]
assert "*dblptr = *((double *) itm_val)" in body
print("Mrm callback float inputs are checked before mutation")
