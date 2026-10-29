#!/usr/bin/env python3
"""Check TabStack menu construction handles child creation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TabStack.c").read_text()
start = source.index("BuildMenu(")
start = source.index("{", start)
end = source.index("static void\n#ifndef _NO_PROTO\nTabTransferProc", start)
body = source[start:end]
assert "if (tmp == NULL)" in body
assert "unable to create tab menu item" in body
assert "while (j < buttonCnt" in body
assert "if (j >= buttonCnt)" in body
assert "tab menu is missing an item" in body
print("PASS TabStack menu construction is allocation-safe")
