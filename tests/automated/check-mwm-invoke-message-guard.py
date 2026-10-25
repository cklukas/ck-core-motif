#!/usr/bin/env python3
"""Check that MWM invoke messages handle a missing UTM widget."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "clients/mwm/WmCmd.c").read_text()


def main() -> None:
    assert "if (ACTIVE_PSD == NULL || ACTIVE_PSD->utmShell == NULL)" in SOURCE
    assert "XtFree ((char *) fulldata);" in SOURCE
    assert "UTMSendMessage(ACTIVE_PSD->utmShell" in SOURCE


if __name__ == "__main__":
    main()
