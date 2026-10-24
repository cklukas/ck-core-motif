#!/usr/bin/env python3
"""Check that UIL URM output strings stay within their local buffers."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilP2Out.c").read_text()
assert "buffer[64];" in source
assert 'snprintf(buffer, sizeof(buffer), "widget-%d-%d-%d"' in source
assert 'snprintf(buffer, sizeof(buffer), "while %s encountered %s"' in source
assert 'sprintf(buffer, "widget-%d-%d-%d"' not in source
assert 'sprintf(buffer, "while %s encountered %s"' not in source
print("UIL generated widget and URM diagnostic output is bounded")
