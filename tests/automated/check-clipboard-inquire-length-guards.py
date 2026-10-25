#!/usr/bin/env python3
"""Check XmClipboardInquireLength input and header guards."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "lib/Xm/CutPaste.c").read_text()


def main() -> None:
    start = SOURCE.index("int \nXmClipboardInquireLength(")
    end = SOURCE.index("int \nXmClipboardInquirePendingItems(", start)
    function = SOURCE[start:end]

    assert "if (format == NULL)" in function
    assert "return ClipboardBadFormat;" in function
    assert "header = ClipboardOpen( display, 0 );" in function
    assert "if (header == NULL)" in function
    assert function.index("if (header == NULL)") < function.index(
        "InitializeSelection( display, header"
    )


if __name__ == "__main__":
    main()
