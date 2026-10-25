#!/usr/bin/env python3
"""Check that protocol property publication skips null manager/entry slots."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "Protocols.c"
text = source.read_text(encoding="utf-8")
for needle in (
    "if (p_mgr == NULL ||",
    "if (protocols[i] != NULL && protocols[i]->protocol.active)",
    "if (ap_mgr->protocol_mgrs[i] != NULL)",
):
    if needle not in text:
        raise SystemExit(f"protocol publication guard is missing: {needle}")

print("protocol publication null-slot guards present")
