#!/usr/bin/env python3
"""Check Container node creation, insertion, and view traversal inputs."""
from pathlib import Path


source = (Path(__file__).resolve().parents[2] / "lib" / "Xm" / "Container.c").read_text(
    encoding="utf-8"
)
change_start = source.rindex("ChangeView(\n")
change_region = source[change_start : source.index("\n}\n", change_start)]
assert "if (wid == NULL)" in change_region
assert "if (cwid == NULL)" in change_region

new_start = source.rindex("NewNode(\n")
new_region = source[new_start : source.index("\n}\n", new_start)]
assert "if (cwid == NULL)" in new_region
assert "if (c == NULL)" in new_region

insert_start = source.rindex("InsertNode(\n")
insert_region = source[insert_start : source.index("\n}\n", insert_start)]
assert "if (cwid == NULL)" in insert_region
assert "if (cw == NULL || c == NULL)" in insert_region
assert "if (pc == NULL)" in insert_region

print("PASS Container node lifecycle and view traversal validate inputs")
