#!/usr/bin/env python3
"""Check clipboard event-handler record and callback validation."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "lib/Xm/CutPaste.c").read_text()


def main() -> None:
    start = SOURCE.index("static void \nClipboardEventHandler(")
    end = SOURCE.index("static int \nClipboardFindItem(", start)
    handler = SOURCE[start:end]

    assert "formatitem == NULL" in handler
    assert "formatlength < sizeof(ClipboardFormatItemRec)" in handler
    assert "if (cbProcTable == NULL)" in handler
    assert "XtFree( (char *)formatitem );" in handler
    assert "formatitem->cutByNameCBIndex < maxCbProcs" in handler
    assert "header = NULL;" in handler
    assert "hlength < sizeof(ClipboardHeaderRec)" in handler
    assert handler.index("header->recopyId = 0;") > handler.index(
        "hlength < sizeof(ClipboardHeaderRec)"
    )


if __name__ == "__main__":
    main()
