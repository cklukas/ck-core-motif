#!/usr/bin/env python3
"""Ensure Mrm callback writers allocate their terminating callback item."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrwr.c").read_text(
    encoding="utf-8"
)
for name in ("UrmCWRSetArgCallback", "UrmCWRSetCreationCallback"):
    start = text.index(name + " (")
    end = text.index("\n}\n", start)
    body = text[start:end]
    assert "item_count = (size_t)nitems + 1" in body
    assert "descsiz = sizeof(RGMCallbackDesc) + item_count*sizeof(RGMCallbackItem)" in body
    assert "cbdesc->item[nitems].runtime.callback.callback = NULL" in body
print("Mrm callback writers allocate the terminator")
