#!/usr/bin/env python3
"""Check that UIL WMD path formatting sizes its expanded result exactly."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilDB.c").read_text()
start = source.index("String init_wmd_path(filename)")
end = source.index("\n}\n", start) + 2
region = source[start:end]

assert "snprintf(NULL, 0, WMDPATH_DEFAULT" in region
assert "snprintf(NULL, 0, XAPPLRES_DEFAULT" in region
assert "(size_t)path_length + 1" in region
assert "sprintf( wmd_path" not in region
assert "sprintf(wmd_path" not in region
print("UIL WMD path expansion is sized and formatted safely")
