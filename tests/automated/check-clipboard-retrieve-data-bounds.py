#!/usr/bin/env python3
"""Check ClipboardRetrieve data-result and offset bounds."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "lib/Xm/CutPaste.c").read_text()


def main() -> None:
    start = SOURCE.index("ClipboardRetrieve(Display")
    end = SOURCE.index("XmClipboardInquireCount(", start)
    function = SOURCE[start:end]

    assert "formatdata = NULL;" in function
    assert "status = ClipboardFindItem" in function
    assert "copiedlength > formatdatalength" in function
    assert "formatdata == NULL && loc_outlength != 0" in function
    assert "if (loc_outlength != 0)" in function
    assert function.index("remaininglength = formatdatalength - copiedlength") > function.index(
        "copiedlength > formatdatalength"
    )


if __name__ == "__main__":
    main()
