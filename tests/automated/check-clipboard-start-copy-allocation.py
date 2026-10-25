#!/usr/bin/env python3
"""Check that XmClipboardStartCopy handles its data-item allocation."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "lib/Xm/CutPaste.c").read_text()


def main() -> None:
    start = SOURCE.index("XmClipboardStartCopy(")
    end = SOURCE.index("XmClipboardCopy(", start)
    function = SOURCE[start:end]

    allocation = function.index("itemheader = (ClipboardDataItem)XtMalloc")
    guard = function.index("if (itemheader == NULL)", allocation)
    header_update = function.index("header->selectionTimestamp = timestamp", guard)
    assert guard > allocation
    assert "ClipboardClose( display, header );" in function[guard:header_update]
    assert "ClipboardUnlock( display, window, 0 );" in function[guard:header_update]
    assert "return ClipboardFail;" in function[guard:header_update]


if __name__ == "__main__":
    main()
