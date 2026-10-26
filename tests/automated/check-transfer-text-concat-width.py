#!/usr/bin/env python3
"""Check selection text concatenation stays within Xt's Cardinal width."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text(encoding="utf-8")
start = source.index("_XmTextToLocaleText")
region = source[start:source.index("void\n_XmConvertComplete", start)]
assert "malloc_size >= (size_t)UINT_MAX" in region
assert region.index("malloc_size >= (size_t)UINT_MAX") < region.index("XtMalloc (malloc_size + 1)")
print("PASS selection text concatenation bounds Xt allocation width")
