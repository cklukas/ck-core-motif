#!/usr/bin/env python3
"""Check UIL diagnostic pointer lines stay inside their fixed buffer."""

from pathlib import Path


source = Path("clients/uil/UilLstLst.c").read_text()
start = source.index("void\tlst_output_message_ptr_line")
end = source.index("\n}\n", start) + 2
body = source[start:end]

assert "max_pos = (int)(sizeof(buffer) - 3);" in body
assert "pos < max_pos" in body
assert "ptr_buffer[ pos ] = 0;" in body

print("UIL diagnostic pointer lines are bounded")
