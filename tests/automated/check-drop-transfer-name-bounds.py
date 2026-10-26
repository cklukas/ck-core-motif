#!/usr/bin/env python3
"""Check drop-transfer widget names use an unsigned bounded counter."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DropTrans.c").read_text()
start = source.index("StartDropTransfer(\n")
end = source.index("\n}\n", start) + 2
body = source[start:end]

assert "static unsigned int which = 0;" in body
assert 'snprintf(buf, sizeof(buf), "Transfer%u"' in body
assert 'sprintf(buf, "Transfer%d"' not in body
print("PASS drop-transfer names use bounded unsigned formatting")
