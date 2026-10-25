#!/usr/bin/env python3
"""Check that ClipboardRetrieveItem handles output allocation failure."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "lib/Xm/CutPaste.c").read_text()


def main() -> None:
    start = SOURCE.index("static int \nClipboardRetrieveItem(")
    end = SOURCE.index("static void \nClipboardReplaceItem(", start)
    helper = SOURCE[start:end]

    assert "if (*outlength == 0)" in helper
    assert "if (*outlength != 0 && clipboard_pointer == NULL)" in helper
    assert "*outpointer = NULL;" in helper
    assert "*outlength = 0;" in helper
    assert "return ClipboardFail;" in helper
    assert "if (ret_value == ClipboardSuccess && loclength != 0)" in helper


if __name__ == "__main__":
    main()
