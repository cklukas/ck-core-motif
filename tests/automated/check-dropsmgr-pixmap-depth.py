#!/usr/bin/env python3
"""Check DropSMgr does not write an int through a Cardinal field pointer."""
from pathlib import Path

source = Path(__file__).resolve().parents[2] / "lib" / "Xm" / "DropSMgr.c"
text = source.read_text(encoding="utf-8")
if "(int*)&(fullInfoRec.animation_pixmap_depth)" in text:
    raise SystemExit("CreateInfo still aliases an int pointer onto Cardinal storage")
if "(int*)&(full_info->animation_pixmap_depth)" in text:
    raise SystemExit("UpdateInfo still aliases an int pointer onto Cardinal storage")
for marker in ("fullInfoRec.animation_pixmap", "full_info->animation_pixmap"):
    start = text.index("XmeGetPixmapData(XtScreen", text.index(marker))
    end = text.index(");", start)
    call = text[start:end]
    if "&pixmap_depth" not in call:
        raise SystemExit(f"{marker} lookup does not use local int depth")
print("DropSMgr pixmap depth lookups use local int storage")
