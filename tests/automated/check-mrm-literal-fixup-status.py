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

icon_start = body.index("case MrmRtypeIconImage:")
icon_end = body.index("case MrmRtypeFontList:", icon_start)
icon_body = body[icon_start:icon_end]
load = icon_body.index("result = Urm__CW_LoadIconImage")
assert "if (result != MrmSUCCESS)" in icon_body[load:]
assert "UrmFreeResourceContext" in icon_body[load:]
assert "UrmPlistFree(*ctxlist);" in icon_body[load:]
assert "*ctxlist = NULL;" in icon_body[load:]

print("Mrm literal loader cleans failed icon and fixup conversions")
