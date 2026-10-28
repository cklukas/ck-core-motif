#!/usr/bin/env python3
"""Ensure Mrm text-vector writers allocate their terminating entry."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrwr.c").read_text(
    encoding="utf-8"
)
for name, count_name in (("UrmCWRSetArgChar8Vec", "num_stg"),
                         ("UrmCWRSetArgCStringVec", "num_cstg")):
    start = text.index(name + " (")
    end = text.index("\n}\n", start)
    body = text[start:end]
    assert f"item_count = (size_t){count_name} + 1" in body
    assert "vecsiz = sizeof(RGMTextVector) + item_count*sizeof(RGMTextEntry)" in body
    assert f"vecptr->item[{count_name}].pointer = NULL" in body
print("Mrm text-vector writers allocate the terminator")
