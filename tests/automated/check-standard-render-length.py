#!/usr/bin/env python3
"""Ensure render-table conversion lengths preserve the public return width."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("} else if (atoms[XmA_MOTIF_RENDER_TABLE]")
end = source.index("} else if (atoms[XmA_MOTIF_ENCODING_REGISTRY]", start)
body = source[start:end]

assert "unsigned int size;" in body
assert "size = XmRenderTableCvtToProp(w, table, &value);" in body
assert "cs -> length = size;" in body
assert "\n    int size;" not in body

print("PASS standard render-table length preserves unsigned width")
