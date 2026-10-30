#!/usr/bin/env python3
"""Ensure DataField drop transfer initializes its widget before querying it."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataF.c").read_text(
    encoding="utf-8"
)
start = source.rindex("df_DropTransferCallback(")
body = source[start:source.index("\n}\n", start) + 2]
assert "Boolean local;" in body
assign = body.index("tf = (XmDataFieldWidget) transfer_rec->widget;")
init = body.index("local = XmTextF_has_primary(tf);")
assert assign < init
assert "Boolean local = XmTextF_has_primary(tf);" not in body
print("PASS DataField drop callback initializes widget before local-state query")
