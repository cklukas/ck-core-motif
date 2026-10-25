from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "uil" / "UilP2Out.c").read_text()

assert "size_t row_bytes" in source
assert "size_t pixel_bytes" in source
assert "out_add_value_size(&size, pixel_bytes);" in source
