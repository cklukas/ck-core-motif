#!/usr/bin/env python3
"""Ensure argument float values are validated before descriptor mutation."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrwr.c").read_text(
    encoding="utf-8"
)
start = text.index("UrmCWRSetArgValue (")
body = text[start:text.index("\n}\n", start)]
guard = "if ((type == MrmRtypeFloat ||"
guard_pos = body.index(guard)
rep_type_pos = body.index("argptr->arg_val.rep_type = type", guard_pos)
assert "arg_val == 0" in body[guard_pos:rep_type_pos]
assert "case MrmRtypeFloat:" in body
print("Mrm argument float inputs are checked before descriptor mutation")
