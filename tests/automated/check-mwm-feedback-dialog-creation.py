#!/usr/bin/env python3
"""Check that MWM confirmation dialogs handle child creation failures."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "clients/mwm/WmFeedback.c").read_text()


def main() -> None:
    assert "if (dialogShellW == NULL)" in SOURCE
    assert "if (pSD->confirmboxW[nbr] == NULL)" in SOURCE
    assert "okButtonW = XmMessageBoxGetChild" in SOURCE
    assert "cancelButtonW = XmMessageBoxGetChild" in SOURCE
    assert "helpButtonW = XmMessageBoxGetChild" in SOURCE
    assert "if (okButtonW == NULL || cancelButtonW == NULL || helpButtonW == NULL)" in SOURCE
    assert "pSD->confirmboxW[nbr] = NULL;" in SOURCE
    assert "messageString != NULL && defaultMessageString != NULL" in SOURCE


if __name__ == "__main__":
    main()
