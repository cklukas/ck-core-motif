#!/usr/bin/env python3
"""Check UIL source records bound flexible-tail allocation arithmetic."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSrcSrc.c").read_text()
diag = source[source.index("src_append_diag_info") : source.index("src_append_machine_code")]
machine = source[source.index("src_append_machine_code") :]

assert "size_t                          l_msg_length;" in diag
assert "l_msg_length > SIZE_MAX - sizeof(src_message_item_type)" in diag
assert "if (az_msg_item == NULL)" in diag
assert "size_t                      l_text_len;" in machine
assert "l_code_len < 0" in machine
assert "allocation_size" in machine
assert "if (az_code_item == NULL)" in machine
print("PASS UIL source-record allocations reject overflow and failure")
