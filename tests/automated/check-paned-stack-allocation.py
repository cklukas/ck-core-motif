#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/Paned.c").read_text()
start = source.index("static void\nPushPaneStack")
end = source.index("static void\nGetPaneStack", start)
region = source[start:end]
assert "XtMalloc(sizeof(PaneStack))" in region
assert "if (stack == NULL)" in region
assert "return;" in region
print("PanedWindow pane-stack allocation guard present")
