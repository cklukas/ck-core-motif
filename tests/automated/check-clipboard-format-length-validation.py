#!/usr/bin/env python3
"""Check clipboard format-length property validation."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "lib/Xm/CutPaste.c").read_text()


def main() -> None:
    start = SOURCE.index("static int \nClipboardGetLenFromFormat(")
    end = SOURCE.index("static ClipboardHeader \nClipboardOpen(", start)
    function = SOURCE[start:end]

    assert "if (format_atom == None)" in function
    assert "*format_length = 8;" in function
    assert "format != 32" in function
    assert "outlength < 1" in function
    assert function.index("if ( outpointer == 0") < function.index(
        "long stored_length = *((long *)outpointer)"
    )


if __name__ == "__main__":
    main()
