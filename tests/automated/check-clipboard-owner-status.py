#!/usr/bin/env python3
"""Check that clipboard ownership failure is reported to the caller."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("XmeClipboardSource(")
end = source.index("/*************************************************************/\n/* Transfer routine section", start)
region = source[start:end]
assert "selection_status = XtOwnSelection" in region
assert "return False;" in region
print("clipboard ownership failure is reported as False")
