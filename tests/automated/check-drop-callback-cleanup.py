#!/usr/bin/env python3
"""Check drop callback cleanup clears freed destination data."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("static void \nDeleteDropCBStruct")
end = source.index("\n/*ARGSUSED*/\nstatic void \nDropDestinationHandler", start)
body = source[start:end]

assert "destination_data != NULL" in body
assert "XtFree((char*) tc -> callback_struct -> destination_data);" in body
assert "destination_data = NULL;" in body

print("Drop callback cleanup is idempotent")
