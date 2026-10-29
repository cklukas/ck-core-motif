#!/usr/bin/env python3
"""Check oversized AutoSuggest input is rejected before existing items are freed."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "AutoSuggestBox.c").read_text()
start = source.index("Boolean XmAutoSuggestBoxSetSuggestions")
end = source.index("Cardinal XmAutoSuggestBoxGetSuggestionCount", start)
function = source[start:end]
guard = function.index("SIZE_MAX / sizeof(*box->auto_suggest.items)")
free = function.index("free_items(box)")
assert guard < free
assert "(count && !items)" in function
print("AutoSuggestBox rejects oversized suggestion arrays before replacing existing items")
