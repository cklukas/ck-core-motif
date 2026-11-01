#!/usr/bin/env python3
"""Check the merged modern implementation contract; native suites exercise behavior."""
from pathlib import Path
source = (Path(__file__).parents[2] / "lib/Xm/AutoSuggestBox.c").read_text()
s = "".join(source.split())
assert "if(!box->auto_suggest.search)" in s
assert "if(!box->auto_suggest.list)" in s
assert "if(box->auto_suggest.search!=NULL)" in s
assert "if(!box->auto_suggest.search||!XtIsRealized(box->auto_suggest.search))return;" in s
print("PASS auto-suggest-child-failure-guards")
