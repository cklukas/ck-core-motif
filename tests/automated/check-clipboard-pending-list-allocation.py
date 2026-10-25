#!/usr/bin/env python3
"""Check that pending clipboard-list allocation is checked."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "lib/Xm/CutPaste.c").read_text()


def main() -> None:
    start = SOURCE.index("XmClipboardInquirePendingItems(")
    end = SOURCE.index("XmClipboardRegisterFormat(", start)
    function = SOURCE[start:end]

    assert "if (header->currItems == 0)" in function
    assert "(size_t)-1 / sizeof(XmClipboardPendingRec)" in function
    assert "if (header->currItems != 0 && itemlist == NULL)" in function
    assert "ClipboardClose( display, header );" in function
    assert "ClipboardUnlock( display, window, 0 );" in function
    assert "return ClipboardFail;" in function
    assert function.index("if (header->currItems != 0 && itemlist == NULL)") < function.index(
        "/* run through all the items"
    )


if __name__ == "__main__":
    main()
