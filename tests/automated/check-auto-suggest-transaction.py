#!/usr/bin/env python3
"""Ensure suggestion replacement is built before visible state is retired."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "AutoSuggestBox.c").read_text(
    encoding="utf-8"
)
start = source.index("Boolean XmAutoSuggestBoxSetSuggestions")
end = source.index("\nCardinal XmAutoSuggestBoxGetSuggestionCount", start)
body = source[start:end]

assert "XmAutoSuggestStoredItem *replacement = NULL;" in body
assert "XmString no_results = NULL;" in body
assert "free_items(box);" in body
assert body.index("if (!replacement)") < body.index("free_items(box);")
assert body.index("if (!replacement[i].id || !label)") < body.index("free_items(box);")
assert body.index("if (!no_results)") < body.index("free_items(box);")
assert "free_item_array(replacement, i + 1);" in body
assert "box->auto_suggest.items = replacement;" in body
print("PASS AutoSuggestBox suggestion replacement is transactional")
