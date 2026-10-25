from pathlib import Path
import os

source = Path(os.environ["MOTIF_SOURCE"])
text = source.read_text()
start = text.index("Create an additional arglist entry")
end = text.index("Now set any pixmap arguments", start)
block = text[start:end]
assert "case MrmRtypeIntegerVector:" in block
assert "default:" in block, "related-code handling must reject non-vector values"
assert block.index("default:") < block.index("Urm__UncompressCode"), "invalid related values must not be decompressed"
assert "continue;" in block or "continue ;" in block
print("Mrm related codes only append vector-count arguments")
