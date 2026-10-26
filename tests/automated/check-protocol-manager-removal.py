#!/usr/bin/env python3
"""Check that protocol-manager teardown updates ownership counts safely."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "Protocols.c"
text = source.read_text(encoding="utf-8")
for needle in (
    "while (ap_mgr->num_protocol_mgrs != 0 &&",
    "if (mgr_index == ap_mgr->num_protocol_mgrs)",
    "ap_mgr->num_protocol_mgrs--;",
):
    if needle not in text:
        raise SystemExit(f"protocol-manager teardown guard is missing: {needle}")

print("protocol-manager teardown guards present")
