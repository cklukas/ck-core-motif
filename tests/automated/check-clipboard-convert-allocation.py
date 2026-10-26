#!/usr/bin/env python3
"""Check clipboard selection conversion allocation and cleanup."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "lib/Xm/CutPaste.c").read_text()


def main() -> None:
    start = SOURCE.index("static Boolean\nClipboardConvertProc")
    end = SOURCE.index("InitializeSelection(\n", start)
    function = SOURCE[start:end]

    allocation = function.index("*value = XtMalloc")
    assert "*size == 0 || *size > UINT_MAX" in function[:allocation]
    assert "XtMalloc( (Cardinal) *size )" in function[allocation:]
    guard = function.index("if (*value == NULL)", allocation)
    retrieve = function.index("ClipboardRetrieve(display", guard)
    assert "*size = 0;" in function[guard:retrieve]
    assert "XtFree( (char *) *value );" in function[retrieve:]
    assert "*value = NULL;" in function[retrieve:]
    assert function.index("*value = NULL;", retrieve) < function.index("goto done;", retrieve)


if __name__ == "__main__":
    main()
