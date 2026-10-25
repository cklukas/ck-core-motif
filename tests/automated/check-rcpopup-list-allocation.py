#!/usr/bin/env python3
"""Check that RCPopup table and per-widget list allocation is guarded."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "RCPopup.c").read_text()
start = source.index("XmAddToPostFromList(")
start = source.index("if (! popup_table)", start)
end = source.index("\n      /* Add to list", start)
region = source[start:end]

assert "if (!popup_table)" in region
assert "_XmProcessUnlock();" in region
assert "_XmAppUnlock(app);" in region
assert "if (list == NULL)" in region
assert region.index("if (list == NULL)") < region.index("list -> popups")

print("PASS RCPopup table and list allocation failures are guarded")
