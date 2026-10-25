#!/usr/bin/env python3
"""Check that clipboard inquiry no-data paths release locks."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "lib/Xm/CutPaste.c").read_text()


def main() -> None:
    assert SOURCE.count("ClipboardClose (display, header);") >= 3
    assert SOURCE.count("ClipboardUnlock (display, window, 0);") >= 3
    assert "return ClipboardNoData;" in SOURCE
    assert SOURCE.count("ClipboardClose (display, header);") >= 3
    assert SOURCE.count("ClipboardUnlock (display, window, 0);") >= 3


if __name__ == "__main__":
    main()
