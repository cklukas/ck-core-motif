#!/usr/bin/env python3
"""Check Container activation validates event and parameter-count inputs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.index("ContainerActivate(\n        Widget")
end = source.index("ContainerExpandOrCollapse(\n        Widget", start)
region = source[start:end]
guard = "if (event == NULL || num_params == NULL)"
assert guard in region
assert region.index(guard) < region.index("*num_params", region.index(guard))
assert region.index(guard) < region.index("CallActionCB")

print("PASS Container activation validates action inputs")
