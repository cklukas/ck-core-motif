#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/IconButton.c").read_text()
start = source.index("static void\nAddPixCache")
end = source.index("/*\tFunction Name: IncPixCache", start)
region = source[start:end]
assert "if (entry == NULL)" in region
assert "if (pix_cache_list == NULL)" in region
assert "if (_XmListAddBefore" in region
assert "_XmListFree(pix_cache_list)" in region
assert "created_list" in region
print("IconButton pixel-cache allocation guards present")
