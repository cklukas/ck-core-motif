#!/usr/bin/env python3
"""Check FileSelectionBox directory prefixes retain size_t lengths."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "FileSB.c").read_text()
assert source.count("size_t          dirLen ;") >= 2
assert "dirLen = strlen( dir) ;" in source
assert "dirLen = strlen( qualifiedDir) ;" in source
assert "(fileList[Index])[dirLen]" in source
assert "(dirList[Index])[dirLen]" in source

print("PASS FileSelectionBox directory lengths preserve size_t width")
