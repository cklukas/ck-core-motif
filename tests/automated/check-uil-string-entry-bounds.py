#!/usr/bin/env python3
from pathlib import Path

source = Path("clients/uil/UilLexAna.c").read_text()
start = source.index("sym_value_entry_type *create_str_entry")
end = source.index("node->header.az_src_rec", start)
block = source[start:end]

assert "if (l_size < 0 || l_size == INT_MAX)" in block
assert 'XtError("UIL string value is too large");' in block
assert "return NULL;" in block
assert "XtCalloc(1, l_size + 1)" in block
