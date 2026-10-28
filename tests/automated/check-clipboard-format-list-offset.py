#!/usr/bin/env python3
"""Check clipboard format-ID lists begin after their owning record."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
assert source.count("<\n\t    sizeof(ClipboardDataItemRec)") >= 1
assert "format_offset < sizeof(ClipboardDataItemRec)" in source
assert "formatIdList * CONVERT_32_FACTOR > reclength" in source

print("Clipboard format-ID lists validate their lower offset bound")
