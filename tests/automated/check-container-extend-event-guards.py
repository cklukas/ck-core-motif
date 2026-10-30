#!/usr/bin/env python3
"""Check Container keyboard extend rejects a missing event."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.index("ContainerExtend(\n        Widget")
end = source.index("ContainerMoveCursor(\n        Widget", start)
region = source[start:end]
guard = "if (event == NULL)"
assert guard in region
assert region.index(guard) < region.index("event->xbutton")

print("PASS Container keyboard extend checks its event")
