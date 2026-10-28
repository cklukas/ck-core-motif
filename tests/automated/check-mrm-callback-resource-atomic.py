#!/usr/bin/env python3
"""Ensure callback resource references commit fields after both appends."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrwr.c").read_text(
    encoding="utf-8"
)
start = text.index("UrmCWRSetCallbackItemRes (")
body = text[start:text.index("\n}\n", start)]
routine_append = body.index("UrmCWR__AppendString")
resource_append = body.index("UrmCWR__AppendResource")
routine_write = body.index("itmptr->cb_item.routine")
type_write = body.index("itmptr->cb_item.rep_type")
offset_write = body.index("itmptr->cb_item.datum.offset")
assert routine_append < resource_append < routine_write
assert routine_write < type_write < offset_write
print("Mrm callback resource references commit after both appends")
