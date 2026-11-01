#!/usr/bin/env python3
"""Guard transactional suggestion ownership and native-list count bounds."""
from pathlib import Path
source = (Path(__file__).parents[2] / "lib/Xm/AutoSuggestBox.c").read_text()
start = source.index("Boolean XmAutoSuggestBoxSetSuggestions(")
end = source.index("Cardinal XmAutoSuggestBoxGetSuggestionCount(", start)
region = source[start:end]
assert "count > INT_MAX" in region
assert "(size_t)count > SIZE_MAX / sizeof(*replacement)" in region
assert "(size_t)count > (size_t)UINT_MAX / sizeof(*replacement)" in region
assert "if (!replacement || !labels) goto failed" in region
assert "XmStringCopy(items[i].label)" in region
assert "if (!replacement[i].id || !replacement[i].label) goto failed" in region
assert region.index("copied = i + 1") < region.index("free_items(box)")
assert "i < copied" in region
assert "XtFree((char *)replacement); XtFree((char *)labels)" in region
print("PASS AutoSuggestBox bounded transactional model replacement")
