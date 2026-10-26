#!/usr/bin/env python3
"""Check that mwm bitmap paths use measured allocation and bounded output."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmImage.c").read_text()
start = source.index("Pixmap GetNamedPixmap")
end = source.index("\n}\n", start) + 2
region = source[start:end]
assert 'pathLength = snprintf(NULL, 0, "%s/%s"' in region
assert 'XtMalloc((Cardinal)pathLength + 1)' in region
assert 'if (pathLength < 0 || (size_t)pathLength >= UINT_MAX)' in region
assert 'snprintf(fullPathName, (size_t)pathLength + 1' in region
assert 'sprintf(fullPathName, "%s/%s"' not in region
print("mwm bitmap path construction is bounded")
