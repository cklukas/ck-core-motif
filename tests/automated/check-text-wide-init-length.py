#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/Text.c").read_text()

needle = "for (num_chars=0; tw->text.wc_value[num_chars]!=0L; num_chars++) {"
assert needle in source
assert "if (num_chars == INT_MAX)" in source
