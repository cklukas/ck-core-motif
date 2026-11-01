#!/usr/bin/env python3
"""Check the merged modern implementation contract; native suites exercise behavior."""
from pathlib import Path
source = (Path(__file__).parents[2] / "lib/Xm/AutoSuggestBox.c").read_text()
s = "".join(source.split())
for name in ["query_changed","search_committed","list_selected","search_events","list_events"]:
    start=s.index("staticvoid"+name+"(Widgetwidget,")
    body=s[start:s.index("}",start)]
    assert "!XmIsAutoSuggestBox((Widget)box)||box->core.being_destroyed" in body
print("PASS autosuggest-callback-guards")
