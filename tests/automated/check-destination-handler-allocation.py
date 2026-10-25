#!/usr/bin/env python3
"""Check destination-handler allocation and DnD input failure paths."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("_XmDestinationHandler(")
end = source.index("\nstatic void\nFinishTransfer", start)
region = source[start:end]

assert "if (cbstruct == NULL)" in region
assert "if (cbstruct -> transfer_id == NULL)" in region
assert "if (ds == NULL)" in region
assert "if (malloc_ds == NULL)" in region
assert "FreeTransferID((XtPointer)tc);" in region

start = source.index("GetTransferID(void)")
end = source.index("\nstatic void\nFreeTransferID", start)
pool_region = source[start:end]
assert "if (rval == NULL)" in pool_region
assert "return(NULL);" in pool_region

print("PASS destination-handler allocation checks")
