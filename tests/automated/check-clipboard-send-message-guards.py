#!/usr/bin/env python3
"""Check ClipboardSendMessage input and header guards."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "lib/Xm/CutPaste.c").read_text()


def main() -> None:
    start = SOURCE.index("static int \nClipboardSendMessage(")
    end = SOURCE.index("static int \nClipboardDataIsReady", start)
    function = SOURCE[start:end]

    assert "if (formatptr == NULL)" in function
    assert "root_clipboard_header = NULL;" in function
    assert "status = ClipboardFindItem" in function
    assert "ClipboardHeaderRecordValid(root_clipboard_header, headerlength)" in function
    assert "XtFree( (char *)root_clipboard_header);" in function
    assert function.index("root_clipboard_header->recopyId") > function.index(
        "ClipboardHeaderRecordValid(root_clipboard_header, headerlength)"
    )


if __name__ == "__main__":
    main()
