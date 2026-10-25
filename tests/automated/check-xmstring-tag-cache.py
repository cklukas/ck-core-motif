#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/XmString.c").read_text()
assert "index >= _cache_count" in source
assert "index < 0" in source
assert "char **new_cache" in source
assert "if (new_cache == NULL)" in source
assert "SIZE_MAX / sizeof(char *)" in source
assert "UINT_MAX / sizeof(char *)" in source
assert "(Cardinal)(sizeof(char *) * (size_t)(_cache_count + 1))" in source
assert "_tag_cache == NULL || tag_index < 0" in source
print("XmString tag-cache bounds and growth guards present")
