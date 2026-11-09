#!/usr/bin/env python3
"""Check that UIL diagnostic source locations retain complete text safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilDiags.c").read_text()
start = source.index("src_buffer[ 0 ] = 0;")
end = source.index("    /*\n    **  issue the error", start)
region = source[start:end]

assert region.count("loc_buffer = diag_format(") == 6
assert "snprintf( loc_buffer" not in region
assert "snprintf(loc_buffer" not in region
assert "if (pointer_pos > source_length)" in region
assert "ptr_buffer[pointer_pos + 1] = 0;" in region
print("UIL diagnostic source locations use dynamic bounded formatting")
