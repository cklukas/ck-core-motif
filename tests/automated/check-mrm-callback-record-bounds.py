#!/usr/bin/env python3
"""Check Mrm creation callback records before callback fixup and iteration."""
import os
from pathlib import Path

source = Path(os.environ.get("MOTIF_SOURCE", Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"))
text = source.read_text(encoding="utf-8")
required = (
    "static Boolean\nUrmCallbackRecordValid",
    "sizeof(OldRGMCallbackDesc) - sizeof(OldRGMCallbackItem)",
    "sizeof(RGMCallbackDesc) - sizeof(RGMCallbackItem)",
    "if (count < 0\n#if SHRT_MAX > RGMListSizeMax\n      || count > RGMListSizeMax\n#endif\n      )",
    "return (size_t)count + 1 <= available / item_size;",
)
for needle in required:
    if needle not in text:
        raise SystemExit(f"missing callback-record bound: {needle}")
if text.count("UrmCallbackRecordValid(widgetrec, widgetrec->creation_offs") != 2:
    raise SystemExit("callback record is not checked in both widget-instance paths")
print("Mrm old and current creation callback records are bounded")
