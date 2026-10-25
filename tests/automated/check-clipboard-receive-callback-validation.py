#!/usr/bin/env python3
"""Check ClipboardReceiveData callback-result validation."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "lib/Xm/CutPaste.c").read_text()


def main() -> None:
    start = SOURCE.index("static void\nClipboardReceiveData")
    end = SOURCE.index("static int \nClipboardRequestDataAndWait", start)
    function = SOURCE[start:end]

    assert "if (info == NULL)" in function
    assert "type != NULL && length != NULL && format != NULL" in function
    assert "*format == 8 || *format == 16 || *format == 32" in function
    assert "value != NULL || *length == 0" in function
    assert "XtFree( (char *)value );" in function
    assert function.index("info -> count = BYTELENGTH") > function.index(
        "*format == 8 || *format == 16 || *format == 32"
    )


if __name__ == "__main__":
    main()
