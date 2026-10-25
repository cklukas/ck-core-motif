#!/usr/bin/env python3
"""Check that protocol lists do not overflow fixed 32-entry buffers."""
from pathlib import Path
source = (Path(__file__).parents[2] / "lib" / "Xm" / "Protocols.c").read_text(encoding="utf-8")
start = source.index("static void\nUpdateProtocolMgrProperty")
update = source[start:source.index("static void \nInstallProtocols", start)]
start = source.index("static void\nRemoveProtocols")
remove = source[start:source.index("PUBLIC INTERFACES", start)]
assert "active_protocols_stack[MAX_PROTOCOLS]" in update
assert "XtMalloc" in update
assert "match_list[MAX_PROTOCOLS]" not in remove
assert "!protocols" in remove
assert "shell == NULL || protocols == NULL" in source
print("PASS protocol list updates and removals are size-safe")
