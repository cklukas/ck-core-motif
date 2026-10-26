#!/usr/bin/env python3
"""Check Text and TextField DnD completion callbacks validate inputs."""

from pathlib import Path


root = Path(__file__).parents[2] / "lib" / "Xm"
for name in ("TextSel.c", "TextFSel.c"):
    source = (root / name).read_text()
    destroy = source.rindex("DropDestroyCB(Widget")
    destroy_body = source[destroy:source.index("\n}\n", destroy)]
    assert "ts != NULL" in destroy_body, name
    start = source.rindex("DropTransferProc(Widget")
    body = source[start:source.index("\nstatic ", start)]
    assert "closure == NULL" in body, name
    assert "transfer_rec->widget == NULL" in body, name
    assert "XmTRANSFER_DONE_FAIL" in body, name

print("Text and TextField DnD callbacks validate transfer inputs")
