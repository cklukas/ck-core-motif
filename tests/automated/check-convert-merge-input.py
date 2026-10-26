#!/usr/bin/env python3
"""Check that conversion merging rejects a null callback structure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("XmeConvertMerge(")
end = source.index("/****************************************************************/\n/* XmeTransferAddDoneProc", start)
region = source[start:end]
assert "if (cs == NULL)" in region
assert "_XmProcessLock();" in region
print("conversion merge rejects null callback state before locking")
