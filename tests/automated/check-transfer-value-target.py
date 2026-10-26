#!/usr/bin/env python3
"""Check that transfer requests reject an invalid target atom."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("XmTransferValue(")
end = source.index("/************************************************************************/\n/* XmTransferStartRequest", start)
region = source[start:end]
assert "tc == NULL || tc->widget == NULL || target == None" in region
print("transfer value requests reject the None target atom")
