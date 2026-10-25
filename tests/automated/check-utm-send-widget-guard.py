#!/usr/bin/env python3
"""Check that UTMSendMessage rejects a null widget at its API boundary."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "clients/mwm/WmWsmLib/utm_send.c").read_text()


def main() -> None:
    assert "if (w == NULL)" in SOURCE
    assert 'UTM Error: cannot send a message without a widget.' in SOURCE
    assert "return;" in SOURCE
    assert "XmeNamedSink(w," in SOURCE


if __name__ == "__main__":
    main()
