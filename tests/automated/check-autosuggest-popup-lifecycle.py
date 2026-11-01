#!/usr/bin/env python3
"""Check the merged modern implementation contract; native suites exercise behavior."""
from pathlib import Path
source = (Path(__file__).parents[2] / "lib/Xm/AutoSuggestBox.c").read_text()
s = "".join(source.split())
assert "if(!box->auto_suggest.popup_shell)return;" in s
assert "XtNdestroyCallback,popup_destroyed,box" in s
assert "box->auto_suggest.popup_shell=NULL;" in s
assert "box->auto_suggest.list=NULL;" in s
print("PASS autosuggest-popup-lifecycle")
