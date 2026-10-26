#!/usr/bin/env python3
"""Check that invalid clipboard target-list results are released."""

from pathlib import Path


source = Path("lib/Xm/Transfer.c").read_text(encoding="utf-8")

assert "Boolean target_list_converted;" in source
assert "target_list_converted = _XmConvertHandler" in source
assert "} else if (target_list_converted == True) {\n    XtFree((char *)value);" in source
assert "targets = (Atom *) value;\n    value = NULL;" in source

print("clipboard target-list cleanup checks passed")
