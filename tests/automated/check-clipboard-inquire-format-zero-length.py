#!/usr/bin/env python3
"""Check zero-length XmClipboardInquireFormat copy handling."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "lib/Xm/CutPaste.c").read_text()


def main() -> None:
    start = SOURCE.index("int \nXmClipboardInquireFormat(")
    end = SOURCE.index("int \nXmClipboardInquireLength(", start)
    function = SOURCE[start:end]

    copy = function.index("strncpy( (char *) buffer")
    assert "if (loc_outlength != 0)" in function[:copy]
    assert function.index("if (loc_outlength != 0)") < copy


if __name__ == "__main__":
    main()
