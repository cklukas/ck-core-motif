#!/usr/bin/env python3
"""Ensure argument resource references commit their type after append."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrwr.c").read_text(
    encoding="utf-8"
)
start = text.index("UrmCWRSetArgResourceRef(")
body = text[start:text.index("\n}\n", start)]
append_pos = body.index("UrmCWR__AppendResource")
type_pos = body.index("argptr->arg_val.rep_type = MrmRtypeResource")
assert type_pos > append_pos
assert body.index("argptr->arg_val.datum.offset = offset", type_pos) > type_pos
print("Mrm argument resource references commit after append")
