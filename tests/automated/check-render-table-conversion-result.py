#!/usr/bin/env python3
"""Check that render-table conversion cannot publish a null non-empty value."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("atoms[XmA_MOTIF_RENDER_TABLE] == cs -> target")
end = source.index("} else if (atoms[XmA_MOTIF_ENCODING_REGISTRY]", start)
region = source[start:end]
assert "size != 0 && value == NULL" in region
assert "cs -> type = None;" in region
assert region.index("size != 0 && value == NULL") < region.index("cs -> type = XA_STRING;")
print("render-table conversion rejects a null non-empty payload")
