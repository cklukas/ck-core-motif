#!/usr/bin/env python3
"""Check that UTF-8 conversion validates its input/output pointers."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "lib/Xm/Xm.c").read_text()


def main() -> None:
    start = SOURCE.index("_XmUtf8ToUcs2")
    end = SOURCE.index("XmeWarning", start)
    body = SOURCE[start:end]
    assert "if (ret_str_len == NULL)" in body
    assert "if (draw_text == NULL && seg_len != 0)" in body
    assert "*ret_str_len = 0;" in body


if __name__ == "__main__":
    main()
