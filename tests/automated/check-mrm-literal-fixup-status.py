#!/usr/bin/env python3
"""Check literal loading does not discard fixup failures."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmlread.c").read_text(
    encoding="utf-8"
)
start = source.index("Urm__FetchLiteral (MrmHierarchy")
end = source.index("MrmFetchLiteral (MrmHierarchy", start)
body = source[start:end]

assert body.count("result = Urm__CW_FixupValue") >= 3
for position in (
    body.index("else \n\t{") + body[body.index("else \n\t{"):].index("result = Urm__CW_FixupValue"),
    body.index("default:\n      result = Urm__CW_FixupValue"),
):
    following = body[position:]
    assert "if (result != MrmSUCCESS)" in following[:300]
    assert "return result;" in following[:300]

print("Mrm literal loader propagates fixup failures")
