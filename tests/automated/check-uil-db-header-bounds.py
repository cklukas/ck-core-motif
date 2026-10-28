#!/usr/bin/env python3
"""Check UIL database headers reject signed table-size/count underflow."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilDB.c").read_text()

assert "if (header.num_items < 0 || header.table_size < 0)" in source
assert "diag_issue_diagnostic( d_bad_database" in source
assert "break;" in source[source.index("if (header.num_items < 0") : source.index("switch (header.table_id)")]
print("UIL database headers reject negative table counts and sizes")
