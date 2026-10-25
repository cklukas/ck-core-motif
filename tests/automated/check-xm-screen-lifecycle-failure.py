#!/usr/bin/env python3
"""Check Xm screen initialization and destruction failure guards."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Screen.c").read_text(encoding="utf-8")
start = source.index("static void \nInitialize(")
end = source.index("/************************************************************************\n *\n *  SetValues", start)
initialize = source[start:end]
assert "if (xmScreen->screen.screenInfo == NULL)" in initialize
start = source.index("static void \nDestroy(")
end = source.index("static void \nInsertChild", start)
destroy = source[start:end]
assert "if (tab != NULL)" in destroy
assert destroy.count("_XmFreeHashTable(tab)") >= 1
print("Xm screen lifecycle handles partial initialization safely")
