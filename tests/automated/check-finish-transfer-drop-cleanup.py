#!/usr/bin/env python3
"""Check final transfer teardown owns drop destination-data cleanup."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("static void\nFinishTransfer")
end = source.index("/****************************************************************/\n/* DropDestinationHandler", start)
body = source[start:end]

assert "CallDoneProcs(wid, tc, &ts);" in body
cleanup = body.index("tc -> callback_struct -> destination_data")
assert cleanup > body.index("CallDoneProcs(wid, tc, &ts);")
assert "XtFree((char *)tc -> callback_struct -> destination_data);" in body
assert "tc -> callback_struct -> destination_data = NULL;" in body
print("Final transfer teardown owns drop cleanup")
