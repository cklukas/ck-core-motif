#!/usr/bin/env python3
"""Check ClipboardClose null and serialized-size guards."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "lib/Xm/CutPaste.c").read_text()


def main() -> None:
    start = SOURCE.index("static void \nClipboardClose(")
    end = SOURCE.index("static void \nClipboardDeleteId(", start)
    function = SOURCE[start:end]

    assert "root_clipboard_header == NULL" in function
    assert "root_clipboard_header->currItems >" in function
    assert "ULONG_MAX - sizeof(ClipboardHeaderRec)" in function
    assert function.index("if (root_clipboard_header == NULL") < function.index(
        "headerlength = sizeof"
    )


if __name__ == "__main__":
    main()
