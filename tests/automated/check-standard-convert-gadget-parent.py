#!/usr/bin/env python3
"""Check scalar standard conversion handles detached gadgets safely."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("XmeStandardConvert(")
end = source.index("Atom\nXmeGetEncodingAtom", start)
body = source[start:end]

assert body.count("if (XmIsGadget(w)) {") == 3
assert body.count("w = XtParent(w);") == 3
assert body.count("if (w == NULL)") >= 3

print("PASS standard conversion guards detached gadget parents")
