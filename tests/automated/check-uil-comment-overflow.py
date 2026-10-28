#!/usr/bin/env python3
from pathlib import Path

source = Path("clients/uil/UilLexAna.c").read_text()
start = source.index("case token_comment:")
end = source.index("case token_eof:", start) if "case token_eof:" in source[start:] else start + 2500
block = source[start:end]

assert 'XtError("UIL comment is too large");' in block
assert block.count("return UILEOF;") >= 2
assert "required = current_length + incoming_length + 2" in block
