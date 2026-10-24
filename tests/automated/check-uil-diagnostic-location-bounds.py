#!/usr/bin/env python3
"""Check that UIL diagnostic source locations use the fixed buffer safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilDiags.c").read_text()
start = source.index("src_buffer[ 0 ] = 0;")
end = source.index("    /*\n    **  issue the error", start)
region = source[start:end]

assert region.count("snprintf( loc_buffer, sizeof(loc_buffer)") == 4
assert region.count("snprintf(loc_buffer, sizeof(loc_buffer)") == 2
assert "sprintf( loc_buffer" not in region
assert "sprintf(loc_buffer" not in region
print("UIL diagnostic source locations are bounded")
