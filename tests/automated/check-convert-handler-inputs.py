#!/usr/bin/env python3
"""Check that the central transfer converter validates all pointer inputs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("_XmConvertHandler(Widget")
end = source.index("/****************************************************************/\n/* DragConvertHandler", start)
region = source[start:end]
assert "wid == NULL" in region
assert "selection == NULL" in region
assert "value == NULL" in region
assert "fmt == NULL" in region
print("central transfer conversion rejects incomplete pointer inputs")
