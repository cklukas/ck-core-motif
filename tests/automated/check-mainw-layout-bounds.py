#!/usr/bin/env python3
"""Check MainWindow layout performs signed size arithmetic before fallback checks."""
from pathlib import Path

source = Path(__file__).resolve().parents[2] / "lib" / "Xm" / "MainW.c"
text = source.read_text(encoding="utf-8")
start = text.index("static void \nLayout(")
end = text.index("\f", start)
body = text[start:end]
if body.count("tmp = (int)mw->core.width -") != 3:
    raise SystemExit("MainWindow width fallback checks are not signed")
if "tmp = (int)mw->core.height -" not in body:
    raise SystemExit("MainWindow command-area height fallback is not signed")
if "(int)mw->core.height - (int)swy -" not in body:
    raise SystemExit("MainWindow separator placement is not signed")
if "sepy = (int)mw->core.height -" not in body:
    raise SystemExit("MainWindow message separator placement is not signed")
print("MainWindow layout bounds use signed arithmetic")
