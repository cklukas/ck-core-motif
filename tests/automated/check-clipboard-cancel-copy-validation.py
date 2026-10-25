#!/usr/bin/env python3
"""Check XmClipboardCancelCopy failure-path validation."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "lib/Xm/CutPaste.c").read_text()


def main() -> None:
    start = SOURCE.index("int \nXmClipboardCancelCopy(")
    end = SOURCE.index("int \nXmClipboardWithdrawFormat(", start)
    function = SOURCE[start:end]

    assert "int status;" in function
    assert "int_ptr = NULL;" in function
    assert "status = ClipboardFindItem" in function
    assert "length < sizeof(long)" in function
    assert "return ClipboardFail;" in function
    assert "header = NULL" not in function
    assert "if (header == NULL)" in function
    assert function.index("*(long *) int_ptr") > function.index(
        "length < sizeof(long)"
    )


if __name__ == "__main__":
    main()
