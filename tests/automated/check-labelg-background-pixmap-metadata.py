#!/usr/bin/env python3
"""Check LabelG background pixmap metadata fallback."""
from pathlib import Path

source = Path(__file__).resolve().parents[2] / "lib" / "Xm" / "LabelG.c"
text = source.read_text(encoding="utf-8")
start = text.index("void\n_XmLabelSetBackgroundGC")
end = text.find("\f", start)
body = text[start:] if end < 0 else text[start:end]
required = [
    "if (XmeGetPixmapData(XtScreen((Widget)lw),",
    "if (depth == 1)",
    "valueMask |= GCFillStyle | GCStipple",
    "valueMask |= GCFillStyle | GCTile",
]
missing = [fragment for fragment in required if fragment not in body]
if missing:
    raise SystemExit("LabelG metadata fallback missing: " + ", ".join(missing))
lookup = body.index("if (XmeGetPixmapData")
font = body.index("if (XmeRenderTableGetDefaultFont")
metadata_block = body[lookup:font]
if metadata_block.count("if (depth == 1)") != 1:
    raise SystemExit("LabelG depth handling is not contained in the lookup-success path")
if "depth = 0" in metadata_block:
    raise SystemExit("LabelG still fabricates a depth after lookup failure")
print("LabelG background pixmap metadata fallback is present")
