#!/usr/bin/env python3
"""Check the merged modern implementation contract; native suites exercise behavior."""
from pathlib import Path
source = (Path(__file__).parents[2] / "lib/Xm/SearchBox.c").read_text()
s = "".join(source.split())
body=s[s.index("voidXmSearchBoxClear("):s.index("voidXmSearchBoxCommit(")]
assert "if(destroyed)return;" in body
assert "if(!emit(widget,XmNsearchClearedCallback,XmSEARCH_REASON_CLEARED,NULL))return;" in body
assert "_XmCallCallbacksUntilDestroyed" in source
print("PASS searchbox-clear-callback-lifetime")
