#!/usr/bin/env python3
"""Check that UIL machine-list text uses bounded format and append helpers."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilLstMac.c").read_text()
assert "UilListAppend(char *buffer, size_t buffer_size" in source
assert "#define UilListFormat(buffer" in source
assert "sprintf (buffer" not in source
assert "sprintf(buffer" not in source
assert "strcat (buffer" not in source
assert "strcat(buffer" not in source
assert source.count("UilListFormat(buffer") >= 70
print("UIL machine-list formatting is bounded")
