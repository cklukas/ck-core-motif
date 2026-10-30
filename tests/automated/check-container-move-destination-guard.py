#!/usr/bin/env python3
"""Check MoveItemCallback rejects transfers without destination state."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.index("MoveItemCallback(\n", source.index("MoveItemCallback (Private Callback Function)"))
end = source.index("DropDoneCallback(", start)
region = source[start:end]
lookup = "ds = _XmTransferGetDestinationCBStruct(cs->transfer_id);"
assert lookup in region
guard = region.index("if (ds == NULL)")
assert region.index(lookup) < guard < region.index("offset = (XPoint *) cs->value;")
assert "return;" in region[guard:region.index("offset = (XPoint *) cs->value;")]
print("PASS MoveItemCallback rejects transfers without destination state")
