#!/usr/bin/env python3
"""Check wide-prototype RepType IDs are range-checked before narrowing."""

from pathlib import Path

text = (Path(__file__).parents[2] / "lib" / "Xm" / "RepType.c").read_text(
    encoding="utf-8"
)

for signature, outcome in (
    ("void\nXmRepTypeAddReverse(", "return;"),
    ("Boolean\nXmRepTypeValidValue(", "return FALSE;"),
    ("XmRepTypeEntry\nXmRepTypeGetRecord(", "return NULL;"),
    ("String *\nXmRepTypeGetNameList(", "return NULL;"),
):
    start = text.index(signature)
    body = text[start:text.index("\n}\n", start) + 3]
    guard = "if (rep_type_id < 0 || rep_type_id >= XmREP_TYPE_INVALID)"
    if guard not in body or outcome not in body[body.index(guard):]:
        raise SystemExit(f"{signature} narrows invalid wide-prototype IDs")

print("wide-prototype representation-type IDs are range-checked")
