#!/usr/bin/env python3
"""Keep UIL machine-code storage and listing growth failure-safe."""
from pathlib import Path

root = Path(__file__).parents[2]
source = (root / "clients/uil/UilSrcSrc.c").read_text(encoding="utf-8")
append = source[source.index("src_append_machine_code") :]

assert "az_src_rec == NULL" in append
assert "l_code_len > 0 && c_code == NULL" in append
assert "az_src_rec->w_machine_code_cnt == USHRT_MAX" in append

listing = (root / "clients/uil/UilLstLst.c").read_text(encoding="utf-8")
start = listing.index("void\tlst_output_machine_code")
end = listing.index("void\tlst_debug_output", start)
body = listing[start:end]
allocation = body.index("new_mc_array =")
failure = body.index("if (new_mc_array == NULL)", allocation)
release = body.index("_free_memory ((char*)mc_array);", allocation)
assign = body.index("mc_array = new_mc_array;", allocation)
assert failure < release < assign
assert "code_len / sizeof (*code_ptr)" in body
assert "code_len % sizeof (*code_ptr)" in body

print("PASS UIL machine-code input and vector growth guards")
