#!/usr/bin/env python3
"""Check Container begin-select rejects missing event inputs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.index("ContainerBeginSelect(\n        Widget")
end = source.index("ContainerButtonMotion(\n        Widget", start)
region = source[start:end]
guard = "if (event == NULL || num_params == NULL)"
assert guard in region
assert region.index(guard) < region.index("event->xbutton")
assert region.index(guard) < region.index("*num_params", region.index(guard))

print("PASS Container begin-select checks event and parameter inputs")
