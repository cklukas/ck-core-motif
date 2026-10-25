#!/usr/bin/env python3
"""Check WSM transient helpers validate detached tree and list state."""
from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmWinList.c"
text = source.read_text()
checks = {
    "FindSubLeaderToTop": ("pcd == NULL", "return (NULL);"),
    "MakeTransientFamilyStackingList": ("windows == NULL || pcdLeader == NULL", "nextWindow == NULL"),
    "LeaderOnTop": ("pcdLeader == NULL", "return (False);"),
}
for name, needles in checks.items():
    signature = {
        "FindSubLeaderToTop": "ClientData *\nFindSubLeaderToTop (",
        "MakeTransientFamilyStackingList": "MakeTransientFamilyStackingList (\n\tWindow *windows",
        "LeaderOnTop": "Boolean\nLeaderOnTop (",
    }[name]
    start = text.index(signature, text.index("#ifdef WSM"))
    block = text[start:start + 1200]
    for needle in needles:
        assert needle in block, (name, needle)
print("WSM transient helpers validate tree and output state")
