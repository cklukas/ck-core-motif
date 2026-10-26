#!/usr/bin/env python3
"""Check Notebook accepts valid uncached pixmap metadata fetches."""
from pathlib import Path

source = Path(__file__).resolve().parents[2] / "lib" / "Xm" / "Notebook.c"
text = source.read_text(encoding="utf-8")
start = text.index("DrawPixmapBinding (")
end = text.index("\n}\n", start) + 3
body = text[start:end]
if "int depth = 0;" not in body:
    raise SystemExit("Notebook pixmap depth is not initialized")
if "if (depth <= 0)" not in body:
    raise SystemExit("Notebook does not reject unresolved pixmap depth")
if "if (!XmeGetPixmapData" not in body:
    raise SystemExit("Notebook does not reject failed pixmap metadata lookup")
if "XmeGetPixmapData(XtScreen(nb), pixmap" not in body:
    raise SystemExit("Notebook pixmap metadata query is missing")
print("Notebook pixmap binding accepts fetched metadata")
