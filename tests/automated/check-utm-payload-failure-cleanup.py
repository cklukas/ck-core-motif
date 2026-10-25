#!/usr/bin/env python3
"""Check that failed UTM packages release their parameter payloads."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "clients/mwm/WmWsmLib/utm_send.c").read_text()


def main() -> None:
    assert SOURCE.count("XtFree((char *)pUtmData->param);") >= 3
    assert "if (!EnqueueUtmData(pUtmData))" in SOURCE
    assert "if (!XmeNamedSink(w," in SOURCE
    assert "if (dcs == NULL)" in SOURCE
    assert "XmTransferSetParameters(dcs->transfer_id" in SOURCE


if __name__ == "__main__":
    main()
