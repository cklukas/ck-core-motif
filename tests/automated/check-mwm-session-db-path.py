#!/usr/bin/env python3
"""Check that mwm session DB paths cannot overflow the fixed pathname buffer."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmXSMP.c").read_text()
start = source.index("#else\n\n    /* Keep the fixed caller buffer bounded")
end = source.index("\n#endif", start)
region = source[start:end]
assert "strlen(dbPath)" in region
assert "dbPathLength < MAXPATHLEN" in region
assert "memcpy(fileNameBuf, dbPath, dbPathLength + 1)" in region
assert "strcpy(fileNameBuf, dtwmFileName)" in region
assert "strcpy(fileNameBuf, (wmGD.dbFileName" not in region
print("mwm session DB path handling is bounded")
