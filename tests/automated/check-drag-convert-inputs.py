#!/usr/bin/env python3
"""Check drag conversion callbacks validate callback inputs."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("DragConvertHandler(")
end = source.index("static int secondary_lock", start)
region = source[start:end]
assert "drag_context == NULL" in region
assert "selection == NULL" in region
assert "type == NULL" in region
assert "cc -> client_data == NULL" in region

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DragC.c").read_text()
for marker, required in (
    ("DropConvertIncrCallback(", "maxLengthRtn == NULL"),
    ("DropConvertCallback(", "formatRtn == NULL"),
):
    start = source.index(marker, source.index(marker) + 1)
    end = source.index("request = XtGetSelectionRequest", start)
    body = source[start:end]
    assert "selection == NULL" in body
    assert "target == NULL" in body
    assert required in body
    assert "return False;" in body

print("PASS DnD conversion frontends validate callback pointers")
