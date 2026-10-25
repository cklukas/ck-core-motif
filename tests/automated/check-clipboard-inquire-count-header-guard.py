#!/usr/bin/env python3
"""Check XmClipboardInquireCount header failure handling."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "lib/Xm/CutPaste.c").read_text()


def main() -> None:
    start = SOURCE.index("int \nXmClipboardInquireCount(")
    end = SOURCE.index("int \nXmClipboardInquireFormat(", start)
    function = SOURCE[start:end]

    assert "header = ClipboardOpen( display, 0 );" in function
    assert "if (header == NULL)" in function
    assert function.index("if (header == NULL)") < function.index(
        "header->copyFromTimestamp"
    )
    assert "ClipboardUnlock( display, window, 0 );" in function
    assert "return ClipboardFail;" in function


if __name__ == "__main__":
    main()
