#!/usr/bin/env python3
"""Check every deferred Mrm bitmap-file path bounds pixmap staging."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c").read_text(
    encoding="utf-8"
)
start = source.index("Urm__CW_CreateArglist (Widget")
end = source.index("long \nUrm__CW_EvaluateValOrOffset", start)
region = source[start:end]
guard = "if (pixargs_cnt >= sizeof(pixargs) / sizeof(pixargs[0]))"
branches = region.split("case MrmRtypeXBitmapFile:")[1:]

assert len(branches) == 2
for branch in branches:
    save = branch.index("savepix = &pixargs[pixargs_cnt]")
    assert guard in branch[:save]
    assert "Urm__UT_Error (\"Urm__CW_CreateArglist\", _MrmMMsg_0048" in branch[:save]
    assert "continue;" in branch[:save]

print("PASS Mrm deferred bitmap paths bound pixmap staging")
