#!/usr/bin/env python3
"""Check transfer request-block allocation failure handling."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("XmTransferValue(")
end = source.index("\nTransferWarning(Widget", start)
value_region = source[start:end]
assert "tb = AddTransferBlock(tc);" in value_region
assert "if (tb == NULL)" in value_region
assert "_XmAppUnlock(app);" in value_region

start = source.index("AddTransferBlock(TransferContext tc)")
end = source.index("\n/* Warning routine */", start)
block_region = source[start:end]
assert "tb = (TransferBlock) XtMalloc(sizeof(TransferBlockRec));" in block_region
assert "if (tb == NULL)" in block_region
assert "return(NULL);" in block_region

print("PASS transfer request-block allocation checks")
