#!/usr/bin/env python3
"""Check that mwm localized system-menu assembly is dynamically sized."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResource.c").read_text()
start = source.index("/* put it together without imposing a fixed translation-size limit */")
end = source.index("\n    if (ResString != NULL)", start)
region = source[start:end]
assert region.count("dsm_length = snprintf(NULL, 0") == 2
assert "char dsm[2048]" not in source
assert "strcat(dsm" not in region
assert "XtFree(dsm);" in region
print("mwm localized system-menu assembly is dynamically bounded")
