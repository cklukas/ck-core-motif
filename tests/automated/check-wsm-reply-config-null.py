#!/usr/bin/env python3
"""Ensure WSM reply packing handles missing configuration metadata."""

from pathlib import Path


text = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "pack.c").read_text(
    encoding="utf-8"
)
start = text.rindex("WinDataSizePacked(Display")
end = text.index("/************************************************************", start)
helper = text[start:end]
assert "fmt_data == NULL" in helper
assert "return(-1);" in helper
start = text.rindex("PackWinData(Display")
end = text.index("/************************************************************", start)
packer = text[start:end]
assert "conf_fmt == NULL" in packer
assert "return(NULL);" in packer
assert "if (size < 0)" in text[text.index("_WSMPackReply"):]
print("WSM reply packing handles missing configuration metadata")
