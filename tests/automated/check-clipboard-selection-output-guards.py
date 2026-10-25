#!/usr/bin/env python3
"""Check ClipboardGetSelection output-pointer validation."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "lib/Xm/CutPaste.c").read_text()


def main() -> None:
    start = SOURCE.index("static int \nClipboardGetSelection(")
    end = SOURCE.index("/*ARGSUSED*/\nstatic void\nClipboardReceiveData", start)
    function = SOURCE[start:end]

    assert "if (value == NULL || type == NULL || size == NULL || format == NULL)" in function
    assert "return FALSE;" in function
    assert function.index("if (value == NULL") < function.index("dest = XtWindowToWidget")


if __name__ == "__main__":
    main()
