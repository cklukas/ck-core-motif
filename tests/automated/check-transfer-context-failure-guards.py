#!/usr/bin/env python3
"""Ensure transfer-context lookup failures are handled by callers."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
assert "if (cc == NULL)" in source
assert "*type = None;" in source
assert "return False;" in source
assert "_XmAppUnlock(app);\n    return;" in source
assert "cc != NULL && cc -> client_data" in source
print("PASS transfer-context lookup failures are guarded")
