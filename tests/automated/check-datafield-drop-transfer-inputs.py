#!/usr/bin/env python3
"""Check DataField drop-transfer callbacks validate transfer inputs."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataF.c").read_text()
start = source.rindex("df_DropTransferCallback(")
end = source.find("\nstatic ", start)
body = source[start:] if end == -1 else source[start:end]
for token in ("w == NULL", "closure == NULL", "type == NULL",
              "length == NULL", "transfer_rec->widget == NULL"):
    assert token in body, token

print("DataField drop-transfer callbacks validate inputs")
