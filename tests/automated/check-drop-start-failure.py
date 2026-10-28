#!/usr/bin/env python3
"""Check failed drop-transfer creation does not publish a stuck request."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("void \nXmTransferValue")
end = source.index("/************************************************************************/\n/* XmTransferStartRequest", start)
body = source[start:end]

assert "static void\nRemoveTransferBlock" in source
failure = body.index("if (tc -> drop_context == NULL)")
assert "RemoveTransferBlock(tc, tb);" in body[failure:]
assert body.index("tc -> outstanding++;", failure) > body.index("return;", failure)
print("Failed drop-transfer creation is rolled back")
