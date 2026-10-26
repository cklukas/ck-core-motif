#!/usr/bin/env python3
"""Check that tear-off snapshots validate GC and pixmap creation."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "TearOff.c"
text = source.read_text()
start = text.index("_XmRestoreTearOffToMenuShell(")
end = text.index("\n}\n", start) + 3
block = text[start:end]

for needle in (
    "if (gc == NULL)",
    "if (shell->core.background_pixmap == None)",
    "XtReleaseGC((Widget) shell, gc);",
):
    if needle not in block:
        raise SystemExit(f"tear-off snapshot path is missing {needle}")

print("tear-off snapshot resources are validated")
