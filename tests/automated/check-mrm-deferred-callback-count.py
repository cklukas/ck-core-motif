#!/usr/bin/env python3
"""Ensure deferred Mrm callback copies include the terminator item."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwrefs.c").read_text(
    encoding="utf-8"
)
start = text.index("Urm__CW_AppendCBSVWidgetRef")
start = text.index("Urm__CW_AppendCBSVWidgetRef (IDBFile", start)
body = text[start:text.index("\n}\n", start)]
assert "cbptr->validation != URMCallbackDescriptorValid" in body
assert "item_count = (size_t)cbptr->count + 1" in body
assert "descsize = sizeof(RGMCallbackDesc) + item_count * sizeof(RGMCallbackItem)" in body
print("Mrm deferred callback copies include the terminator")
