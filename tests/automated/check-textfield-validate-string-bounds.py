#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/TextF.c").read_text()

assert "input_length = strlen(value);" in source
assert "input_length > (size_t)INT_MAX - 1" in source
assert "str_len = (int)input_length;" in source
assert "if (str_len == INT_MAX)" in source
