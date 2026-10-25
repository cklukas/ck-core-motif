#!/usr/bin/env python3
"""Check protocol-manager creation is transactional under allocation failure."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Protocols.c").read_text()
get_start = source.index("GetAllProtocolsMgr(\n        Widget shell )")
get_end = source.index("\n/************************************<+>*************************************", get_start + 1)
get_region = source[get_start:get_end]
assert "if (ap_mgr == NULL)" in get_region

start = source.index("AddProtocolMgr(\n        XmAllProtocolsMgr ap_mgr,")
end = source.index("\n/************************************<+>*************************************", start + 1)
region = source[start:end]
for needle in (
    "p_mgr = XtNew(XmProtocolMgrRec);",
    "if (p_mgr == NULL)",
    "if (resized == NULL)",
    "XtFree((char *)p_mgr);",
    "ap_mgr->protocol_mgrs = resized;",
    "ap_mgr->max_protocol_mgrs = new_max;",
):
    assert needle in region, needle
assert region.index("ap_mgr->protocol_mgrs = resized") < region.index("ap_mgr->max_protocol_mgrs = new_max")
assert "num_protocol_mgrs > max_cardinal - 2" in region

assert source.count("if (p_mgr == NULL) {") >= 2
print("PASS protocol manager allocation is transactional")
