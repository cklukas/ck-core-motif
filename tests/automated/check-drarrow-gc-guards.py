from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DrArrow.c").read_text()

assert "if (!d || (shadow_thick && (!top_gc || !bot_gc))) return ;" in source
