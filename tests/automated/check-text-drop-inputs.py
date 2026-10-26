#!/usr/bin/env python3
"""Check Text drop callbacks reject malformed transfer inputs."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextSel.c").read_text()
start = source.index("HandleDrop(")
end = source.index("\n/* Request targets", start)
body = source[start:end]
assert "cb == NULL || ds == NULL" in body
assert "cb->dragContext == NULL" in body
assert "ds->transfer_id == NULL" in body
assert "numExportTargets != 0 && exportTargets == NULL" in body
assert "XmTRANSFER_DONE_FAIL" in body

print("Text drop callbacks reject malformed inputs")
