#!/usr/bin/env python3
"""Ensure the shared Mrm resource append helper validates its pointers."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrwr.c").read_text(
    encoding="utf-8"
)
start = text.index("UrmCWR__AppendResource (")
body = text[start:text.index("\n}\n", start)]
guard = "if (context_id == NULL || offset == NULL)"
assert guard in body
guard_pos = body.index(guard)
assert body.index("switch ( key_type )", guard_pos) > guard_pos
print("Mrm resource append validates context and output pointers")
