#!/usr/bin/env python3
"""Check the merged modern implementation contract; native suites exercise behavior."""
from pathlib import Path
source = (Path(__file__).parents[2] / "lib/Xm/AutoSuggestBox.c").read_text()
s = "".join(source.split())
body=s[s.index("BooleanXmAutoSuggestBoxSetSuggestions"):s.index("CardinalXmAutoSuggestBoxGetSuggestionCount")]
for guard in ["if(!replacement||!labels)gotofailed;","if(!replacement[i].id||!replacement[i].label)gotofailed;","if(!no_results)gotofailed;"]:
    assert body.index(guard)<body.index("free_items(box)")
assert "XtFree(replacement[i].id);XmStringFree(replacement[i].label);" in body
assert "box->auto_suggest.items=replacement;" in body
print("PASS auto-suggest-transaction")
