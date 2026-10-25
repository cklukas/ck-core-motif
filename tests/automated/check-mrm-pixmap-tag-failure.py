from pathlib import Path
import os

source = Path(os.environ["MOTIF_SOURCE"])
text = source.read_text()
start = text.index("  if ( argptr->tag_code == UilMrmUnknownCode )", text.index("Place resultant Pixmap"))
end = text.index("  *num_used += 1 ;", start)
block = text[start:end]
assert "uncmp_res = Urm__UncompressCode" in block
assert "if (uncmp_res != MrmSUCCESS)" in block
assert "Urm__UT_Error (\"Urm__CW_CreateArglist\"" in block
assert "XFreePixmap (display, pixmap)" in block
print("Mrm deferred pixmap tag failures are reported and cleaned up")
