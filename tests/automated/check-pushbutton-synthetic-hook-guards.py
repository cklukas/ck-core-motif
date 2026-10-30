#!/usr/bin/env python3
"""Check PushButton synthetic pixel hooks validate callback inputs."""

from pathlib import Path


for path, cls in (
    ("lib/Xm/PushB.c", "xmPushButtonWidgetClass"),
    ("lib/Xm/PushBG.c", "xmPushButtonGadgetClass"),
):
    source = Path(path).read_text()
    for function in ("ShowAsDef_ToHorizPix", "ExportHighlightThickness"):
        start = source.rindex(function)
        end = source.index("\n}", start)
        body = source[start:end]
        assert "widget == NULL" in body, (path, function)
        assert "value == NULL" in body, (path, function)
        assert cls in body, (path, function)

print("PushButton synthetic pixel hooks validate widget and value inputs")
