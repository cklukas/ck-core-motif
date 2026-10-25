#!/usr/bin/env python3
"""Check that protocol lists do not overflow fixed 32-entry buffers."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Protocols.c").read_text(encoding="utf-8")
update = source[source.index("static void\nUpdateProtocolMgrProperty"):source.index("static void \nInstallProtocols", source.index("static void\nUpdateProtocolMgrProperty"))]
remove = source[source.index("static void\nRemoveProtocols"):source.index("PUBLIC INTERFACES", source.index("static void\nRemoveProtocols"))]
assert "active_protocols_stack[MAX_PROTOCOLS]" in update
assert "XtMalloc" in update
assert "match_list[MAX_PROTOCOLS]" not in remove
assert "!protocols" in remove
assert "shell == NULL || protocols == NULL" in source
print("PASS protocol list updates and removals are size-safe")
