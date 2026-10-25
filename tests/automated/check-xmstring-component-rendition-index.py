#!/usr/bin/env python3
"""Check that optimized XmString rendition indices are validated."""

from pathlib import Path


source = Path("lib/Xm/XmString.c").read_text()
start = source.index("XmeStringGetComponent(_XmStringContext context,")
end = source.index("\n}\n", start)
function = source[start:end]

assert "_XmStrRendIndex(opt) == REND_INDEX_UNSET" in function
assert "_tag_cache == NULL" in function
assert "_XmStrRendIndex(opt) >= _cache_count" in function
assert "_XmStrContError(context) = True;" in function

print("XmString optimized rendition indices are validated")
