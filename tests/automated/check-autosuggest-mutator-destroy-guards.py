#!/usr/bin/env python3
"""Check the merged modern implementation contract; native suites exercise behavior."""
from pathlib import Path
source = (Path(__file__).parents[2] / "lib/Xm/AutoSuggestBox.c").read_text()
s = "".join(source.split())
assert "(count&&!items)||widget->core.being_destroyed" in s
assert "!XtIsSensitive(widget)||widget->core.being_destroyed" in s
assert "if(destroyed)" in s
assert "alive=_XmCallCallbacksUntilDestroyed(widget,XmNautoSuggestCommittedCallback,&callback);" in s
assert "alive=_XmCallCallbacksUntilDestroyed(widget,XmNautoSuggestDismissedCallback,&callback);" in s
print("PASS autosuggest-mutator-destroy-guards")
