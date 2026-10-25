#!/usr/bin/env python3
"""Check that protocol registration rolls back partial allocation failure."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "Protocols.c"
text = source.read_text(encoding="utf-8")
start = text.index("newSec = XtMalloc(size);")
end = text.index("((XmProtocol) newSec)->protocol.atom", start)
region = text[start:end]
assert "if (newSec == NULL)" in region
assert "while (i > p_mgr->num_protocols)" in region
assert "XtFree((char *)p_mgr->protocols[i]);" in region
assert "return;" in region
print("protocol registration rolls back partial allocation")
