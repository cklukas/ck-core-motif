#!/usr/bin/env python3
"""Check that dialog-shell suffix construction is measured and bounded."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DialogS.c").read_text()
start = source.index("Widget \nXmeCreateClassDialog")
end = source.index("\n}\n", start) + 2
region = source[start:end]
assert 'ds_name_length = snprintf(NULL, 0, "%s%s"' in region
assert 'ds_name = XtMalloc( (size_t)ds_name_length + 1)' in region
assert 'snprintf(ds_name, (size_t)ds_name_length + 1' in region
assert 'strcpy( ds_name, name)' not in region
assert 'strcat( ds_name, XmDIALOG_SUFFIX)' not in region
print("dialog shell suffix construction is bounded")
