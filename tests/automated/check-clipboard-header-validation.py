#!/usr/bin/env python3
"""Check that ClipboardOpen rejects malformed header properties."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "lib/Xm/CutPaste.c").read_text()


def main() -> None:
    start = SOURCE.index("static ClipboardHeader \nClipboardOpen(")
    end = SOURCE.index("static void \nClipboardClose(", start)
    function = SOURCE[start:end]

    assert "headerlength < sizeof(ClipboardHeaderRec)" in function
    assert "root_clipboard_header == NULL" in function
    assert "CleanupHeader( display );" in function
    assert function.count("XtFree( (char *)root_clipboard_header);") >= 2
    assert function.index("header->recordType") > function.index(
        "headerlength < sizeof(ClipboardHeaderRec)"
    )


if __name__ == "__main__":
    main()
