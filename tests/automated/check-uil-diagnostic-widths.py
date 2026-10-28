#!/usr/bin/env python3
"""Check UIL diagnostic position and status-delay widths are explicit."""
from pathlib import Path

root = Path(__file__).parents[2] / "clients" / "uil"
diags = (root / "UilDiags.c").read_text()
srcdef = (root / "UilSrcDef.h").read_text()

assert "(int)strlen( src_buffer )" in diags
assert "status_update_delay > (unsigned int)INT_MAX" in diags
assert "? INT_MAX : (int)Uil_cmd_z_command.status_update_delay" in diags
assert "_key.l_key == (unsigned long)EOF" in srcdef
print("UIL diagnostic and source-position width boundaries are explicit")
