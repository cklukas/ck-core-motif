#!/usr/bin/env python3
"""Check XmClipboardCopy header and generated-ID validation."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "lib/Xm/CutPaste.c").read_text()


def main() -> None:
    start = SOURCE.index("XmClipboardCopy(")
    end = SOURCE.index("XmClipboardEndCopy(", start)
    function = SOURCE[start:end]

    assert "if (header == NULL)" in function
    assert "formatid     = ClipboardGetNewItemId" in function
    assert "formatdataid = ClipboardGetNewItemId" in function
    assert "if (formatid == 0 || formatdataid == 0)" in function
    assert "XtFree( (char *)formatptr);" in function
    assert function.index("if (formatid == 0 || formatdataid == 0)") < function.index(
        "/* put format record id"
    )


if __name__ == "__main__":
    main()
