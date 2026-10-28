#!/usr/bin/env python3
"""Check UIL source-record metadata is validated before compact storage."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSrcSrc.c").read_text()

assert "l_src_pos < 0 || l_src_pos > UCHAR_MAX" in source
assert "allocation_size > UINT_MAX" in source
assert "az_msg_item->b_source_pos = (unsigned char)l_src_pos;" in source
assert "l_offset < 0 || l_offset > USHRT_MAX" in source
assert "l_code_len > USHRT_MAX" in source
assert "az_code_item -> w_code_len = (unsigned short)l_code_len;" in source
print("UIL source-record metadata is bounded before compact storage")
