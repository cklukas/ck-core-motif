#!/usr/bin/env python3
"""Check Container select rejects missing event inputs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.index("ContainerSelect(\n        Widget")
end = source.index("ContainerExtend(\n        Widget", start)
region = source[start:end]
guard = "if (event == NULL || num_params == NULL)"
assert guard in region
assert region.index(guard) < region.index("*num_params", region.index(guard))
assert region.index(guard) < region.index("KBSelect")

print("PASS Container select checks event and parameter inputs")
