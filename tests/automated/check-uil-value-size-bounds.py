from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "uil" / "UilP2Out.c").read_text()

assert "out_add_value_size(int *value_size, size_t amount)" in source
assert "amount > (size_t) INT_MAX - (size_t) *value_size" in source
assert source.count("out_add_value_size(&value_size") >= 9
