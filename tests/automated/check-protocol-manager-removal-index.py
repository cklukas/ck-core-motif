#!/usr/bin/env python3
"""Check protocol-manager removal keeps manager and protocol indices separate."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "Protocols.c"
text = source.read_text(encoding="utf-8")
start = text.index("static void \nRemoveProtocolMgr(")
end = text.index("/************************************<+>*************************************", start)
region = text[start:end]
assert "mgr_index, protocol_index;" in region
assert "for (mgr_index = 0; mgr_index < ap_mgr->num_protocol_mgrs; mgr_index++)" in region
assert "for (protocol_index = 0; protocol_index < p_mgr->num_protocols;" in region
assert "for ( ; mgr_index < ap_mgr->num_protocol_mgrs-1; mgr_index++)" in region
assert "for ( ; i < ap_mgr->num_protocol_mgrs-1; i++)" not in region
print("protocol-manager removal indices are independent")
