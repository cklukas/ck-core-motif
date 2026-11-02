#!/usr/bin/env python3
"""ScrollBar snapshots public callback arrays through the shared safe dispatcher."""
from pathlib import Path
source=(Path(__file__).parents[2]/"lib/Xm/ScrollBar.c").read_text()
start=source.index("static void\nScrollCallback(")
body=source[start:source.index("\n}\n",start)]
assert "XtCallCallbackList" not in body
assert "_XmCallCallbacksUntilDestroyed" in body
assert "callbacks[count].callback" not in source
for resource in ["XmNvalueChangedCallback","XmNincrementCallback","XmNdecrementCallback","XmNpageIncrementCallback","XmNpageDecrementCallback","XmNtoTopCallback","XmNtoBottomCallback","XmNdragCallback"]:
 assert resource in body
print("PASS ScrollBar dispatch uses the shared public-array callback snapshot")
