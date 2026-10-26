#!/usr/bin/env python3
"""Check that DataField wide-value initialization handles OOM."""

from pathlib import Path


source = Path("lib/Xm/DataF.c").read_text(encoding="utf-8")

assert "if (XmTextF_wc_value(new_tf) == NULL)" in source
assert "if (XmTextF_wc_value(new_tf) == NULL) {\n                 diff_values = False;" in source
assert "if (XmTextF_wc_value(new_tf) != NULL)" in source

print("DataField wide-value initialization checks passed")
