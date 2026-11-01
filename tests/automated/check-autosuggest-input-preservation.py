#!/usr/bin/env python3
"""Check the merged modern implementation contract; native suites exercise behavior."""
from pathlib import Path
source = (Path(__file__).parents[2] / "lib/Xm/AutoSuggestBox.c").read_text()
s = "".join(source.split())
body=s[s.index("BooleanXmAutoSuggestBoxSetSuggestions"):s.index("CardinalXmAutoSuggestBoxGetSuggestionCount")]
assert body.index("SIZE_MAX/sizeof(*replacement)")<body.index("free_items(box)")
assert body.index("generation!=box->auto_suggest.generation")<body.index("free_items(box)")
print("PASS autosuggest-input-preservation")
