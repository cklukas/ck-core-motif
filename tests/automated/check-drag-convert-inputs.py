#!/usr/bin/env python3
"""Check that the drag conversion wrapper validates callback inputs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("DragConvertHandler(")
end = source.index("static int secondary_lock", start)
region = source[start:end]
assert "drag_context == NULL" in region
assert "selection == NULL" in region
assert "type == NULL" in region
assert "cc -> client_data == NULL" in region
print("drag conversion callbacks reject incomplete input state")
