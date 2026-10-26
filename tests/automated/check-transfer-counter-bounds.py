#!/usr/bin/env python3
"""Check that transfer counters cannot wrap before a new request."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("XmTransferValue(")
end = source.index("/************************************************************************/\n/* XmTransferStartRequest", start)
region = source[start:end]
assert "tc -> count == INT_MAX" in region
assert "tc -> outstanding == INT_MAX" in region
assert "tc -> outstanding++;" in region
assert "tc -> count++;" in region
print("transfer counters are bounded before request creation")
