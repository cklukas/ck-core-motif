from pathlib import Path
import os

source = Path(os.environ.get(
    "MOTIF_SOURCE", Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"))
text = source.read_text()
start = text.index("case MrmRtypeIconImage:")
end = text.index("case MrmRtypeXBitmapFile:", start)
block = text[start:end]
capacity = "pixargs_cnt >= sizeof(pixargs) / sizeof(pixargs[0])"
assert capacity in block, "icon-image branch must bound the deferred pixmap array"
assert block.index(capacity) < block.index("savepix = &pixargs[pixargs_cnt]"), "capacity must be checked before storing"
assert "_MrmMMsg_0048" in block, "overflow must use the existing bounded-pixmap diagnostic"
print("Mrm icon-image arguments stay within deferred pixmap storage")
