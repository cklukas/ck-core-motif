#!/usr/bin/env python3
"""Check the merged modern implementation contract; native suites exercise behavior."""
from pathlib import Path
source = (Path(__file__).parents[2] / "lib/Xm/AutoSuggestBox.c").read_text()
s = "".join(source.split())
assert "box->auto_suggest.search=NULL;" in s
assert "XtAddCallback(box->auto_suggest.search,XtNdestroyCallback,search_destroyed,box);" in s
assert "XtRemoveCallback(box->auto_suggest.search,XtNdestroyCallback,search_destroyed,box);" in s
assert "!((CoreWidget)box->auto_suggest.search)->core.being_destroyed" not in s
print("PASS autosuggest-search-destroy-order")
