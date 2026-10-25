#!/usr/bin/env python3
"""Check protocol batch growth validates Cardinal arithmetic and reallocations."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "Protocols.c"
text = source.read_text(encoding="utf-8")
start = text.index("static void \nAddProtocols(")
end = text.index("\n/************************************<+>*************************************\n *\n *   RemoveProtocols", start)
region = text[start:end]
for needle in (
    "num_protocols > max_cardinal - p_mgr->num_protocols",
    "add_size > max_cardinal - p_mgr->max_protocols",
    "new_max > max_cardinal / sizeof(XmProtocol)",
    "if (resized == NULL)",
    "p_mgr->max_protocols = new_max",
):
    assert needle in region, needle
print("protocol batch growth bounds counts and reallocation")
