#!/usr/bin/env python3
"""Check deferred clipboard target ownership and snapshot validation."""

from pathlib import Path


source = Path("lib/Xm/Transfer.c").read_text(encoding="utf-8")

assert "Boolean deferred_target_list_converted;" in source
assert "deferred_target_list_converted = _XmConvertHandler" in source
assert "targets = (Atom *) value;\n    value = NULL;" in source
assert "if (value == NULL || size2 == 0)" in source
assert "} else if (deferred_target_list_converted == True) {" in source

print("deferred clipboard cleanup checks passed")
