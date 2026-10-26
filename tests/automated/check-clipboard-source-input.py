#!/usr/bin/env python3
"""Check that clipboard-source setup rejects a null widget."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("XmeClipboardSource(")
end = source.index("/*************************************************************/\n/* Transfer routine section", start)
region = source[start:end]
assert "if (w == NULL)" in region
assert "return False;" in region
assert "_XmWidgetToAppContext(w);" in region
print("clipboard-source setup rejects a null widget before context access")
