#!/usr/bin/env python3
"""Check null-record handling in MWM decoration setup helpers."""

from pathlib import Path


source = Path("clients/mwm/WmCDecor.c").read_text(encoding="utf-8")

checks = (
    ("void CountFrameRectangles (WmScreenData *pSD)",
     "pSD->Num_Title_Ts_Elements", "return;"),
    ("Boolean AllocateFrameDisplayLists (ClientData *pcd)",
     "pcd->decor", "return(FALSE);"),
    ("void InitClientDecoration (WmScreenData *pSD)",
     "CountFrameRectangles(pSD)", "return;"),
)

for start_marker, first_field, return_text in checks:
    start = source.index(start_marker)
    next_start = source.find("\n\f\n/", start + len(start_marker))
    body = source[start:] if next_start < 0 else source[start:next_start]
    assert "if (" in body and "== NULL" in body
    assert return_text in body
    assert body.index("== NULL") < body.index(first_field)

print("PASS MWM decoration setup rejects null records")
