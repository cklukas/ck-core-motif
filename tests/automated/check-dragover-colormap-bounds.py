#!/usr/bin/env python3
"""Guard DragOverShell colormap-window array sizing."""

from pathlib import Path

source = (Path(__file__).resolve().parents[2] / "lib" / "Xm" / "DragOverS.c").read_text()
start = source.index("\nInstallColormap(XmDragOverShellWidget")
end = source.index("\n}\n", start)
region = source[start:end]

assert "#include <limits.h>" in source
assert "#include <stdint.h>" in source
assert "countReturn >= INT_MAX" in region
assert "SIZE_MAX / sizeof(Window)" in region
assert "UINT_MAX / sizeof(Window)" in region
assert "if (windows == NULL)" in region
print("PASS DragOverShell colormap-window allocation bounds")
