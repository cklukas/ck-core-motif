#!/usr/bin/env python3
"""Check that Xmos default resource paths use measured allocation."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xmos.c").read_text()
start = source.index("String\n_XmOSInitPath")
end = source.index("\n}\n", start) + 2
region = source[start:end]
assert region.count("path_length = snprintf(NULL, 0") == 2
assert region.count("path = XtMalloc((size_t)path_length + 1)") == 2
assert region.count("snprintf(path, (size_t)path_length + 1") == 2
assert "sprintf(path, PATH_DEFAULT" not in region
assert "sprintf(path, XAPPLRES_DEFAULT" not in region
print("Xmos default resource path construction is bounded")
