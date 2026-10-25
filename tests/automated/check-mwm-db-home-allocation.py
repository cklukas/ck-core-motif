#!/usr/bin/env python3
"""Check that MWM database naming handles a missing home directory."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmXSMP.c").read_text(encoding="utf-8")
start = source.index("static void\ngetClientDBName")
end = source.index("\n}\n", start) + 2
region = source[start:end]
assert "if (homeDir == NULL)" in region
assert "homeLength = strlen(homeDir);" in region
assert "SIZE_MAX - fileLength - 2 - EXTRA_FN_CHARS" in region
assert "homeLength + fileLength + 2 + EXTRA_FN_CHARS > UINT_MAX" in region
assert "(Cardinal)(homeLength + fileLength + 2 + EXTRA_FN_CHARS)" in region
assert "sprintf(wmGD.dbFileName, \"%s/%s\", homeDir, dtwmFileName);" in region
print("PASS MWM database naming guards home-directory lookup")
