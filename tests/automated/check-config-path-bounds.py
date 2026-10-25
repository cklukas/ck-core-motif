#!/usr/bin/env python3
"""Check that Mwm config paths use checked bounded helpers."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResParse.c").read_text(
    encoding="utf-8"
)
start = source.index("static Boolean\nSetConfigPath")
end = source.index("FILE *FopenConfigFile", start)
section = source[start:end]
assert "snprintf(cfileName, sizeof(cfileName)" in section
assert "snprintf(cfileName + used" in section
assert "cfileName[0] = '\\0';" in section
assert "if (!configPathValid)" in section
assert "strcpy (cfileName" not in section
assert "strncat(cfileName" not in section
# All direct config-buffer writes in FopenConfigFile go through the helpers.
open_start = source.index("FILE *FopenConfigFile")
open_section = source[open_start:]
assert "strcpy (cfileName" not in open_section
assert "strncat(cfileName" not in open_section
assert "strncpy (cfileName" not in open_section
print("Mwm config-file paths use checked bounded helpers")
