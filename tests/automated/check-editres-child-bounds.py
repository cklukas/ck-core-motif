#!/usr/bin/env python3
"""Check Editres child enumeration is bounded and allocation free."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "EditresCom.c").read_text(
    encoding="utf-8"
)

start = source.index("\nChildCount(")
end = source.index("\n/*\tFunction Name: FindMember", start)
region = source[start:end]

assert "total > UINT_MAX ? UINT_MAX" in region
assert "index < cw->composite.num_children" in region
assert "index < parent->core.num_popups" in region
assert "FindChildren" not in source

print("edit-resources child enumeration bounds are guarded")
