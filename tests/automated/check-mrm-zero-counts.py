#!/usr/bin/env python3
"""Check Mrm descriptor builders reject zero-count underflow inputs."""

from pathlib import Path


module = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmmodule.c").read_text(encoding="utf-8")
arglist = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrwr.c").read_text(encoding="utf-8")
if "if ( num_widget >= RGMListSizeMax )" not in module or "if (num_widget == 0)" not in module:
    raise SystemExit("module descriptor zero-count guard missing")
if "if ( nargs == 0 || nargs > RGMListSizeMax ||" not in arglist:
    raise SystemExit("argument-list descriptor zero-count guard missing")

print("Mrm zero-count descriptor inputs are rejected")
