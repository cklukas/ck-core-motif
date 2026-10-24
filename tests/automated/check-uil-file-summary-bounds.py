#!/usr/bin/env python3
"""Check that UIL file-summary formatting truncates by measured capacity."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilLstLst.c").read_text()
start = source.index("for (i = 0; i <= src_l_last_source_file_number; i++)")
end = source.index("\n    lst_output_line( \" \", FALSE );", start)
region = source[start:end]
assert "path_capacity = sizeof(buffer) - 1" in region
assert "if (path_length > path_capacity)" in region
assert "memcpy(buffer + prefix_length" in region
assert "snprintf (buffer, sizeof(buffer)," not in region
print("UIL file-summary paths are bounded without format truncation")
