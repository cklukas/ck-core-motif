#!/usr/bin/env python3
"""Check XmClipboardCopyByName retrieval and buffer validation."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "lib/Xm/CutPaste.c").read_text()


def main() -> None:
    start = SOURCE.index("XmClipboardCopyByName(")
    end = SOURCE.index("XmClipboardUndoCopy(", start)
    function = SOURCE[start:end]

    assert "root_clipboard_header = NULL;" in function
    assert "headerlength < sizeof(ClipboardHeaderRec)" in function
    assert "formatheader = NULL;" in function
    assert "formatlength >= sizeof(ClipboardFormatItemRec)" in function
    assert "formatdataptr = NULL;" in function
    assert "formatdatalength < length" in function
    assert "if (length == 0)" in function
    assert "if (length != 0)" in function
    assert "XtFree( (char *)formatheader);" in function
    assert function.index("memcpy( to_ptr") > function.index(
        "formatdatalength < length"
    )


if __name__ == "__main__":
    main()
