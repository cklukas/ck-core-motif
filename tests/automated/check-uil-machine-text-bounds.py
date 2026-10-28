#!/usr/bin/env python3
"""Check UIL machine-code listing text is bounded before compact storage."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilLstLst.c").read_text()

assert "size_t\t\ttext_size;" in source
assert "text_size = strlen ((char *)text_ptr);" in source
assert "if (text_size > (size_t)(BUF_LEN - TEXT_COL + 1))" in source
assert "text_len = (unsigned short)text_size;" in source
assert "(Cardinal)(sizeof (char *) * (size_t)code_cnt)" in source
print("UIL machine-code listing text and vector widths are bounded")
