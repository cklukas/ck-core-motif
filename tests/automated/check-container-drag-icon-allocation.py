#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/Container.c").read_text()
start = source.index("static\tvoid\nDragStart(")
end = source.index("XSaveContext", start)
region = source[start:end]
assert "dragIconInfo = (DragIconInfo) XtMalloc" in region
assert "if (dragIconInfo == NULL)" in region
assert "if (dragIconInfo == NULL)\n\t  return;" in region
print("Container drag-icon allocation guard present")
