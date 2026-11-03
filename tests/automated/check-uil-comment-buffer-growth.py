#!/usr/bin/env python3
from pathlib import Path

source = Path("clients/uil/UilLexAna.c").read_text()
start = source.index("case token_comment:")
end = source.index("case token_string:", start)
region = source[start:end]
assert "char *new_text = XtRealloc" in region
assert "if (new_text == NULL)" in region
assert region.index("if (new_text == NULL)") < region.index("comment_text = new_text")
assert "return 0;" in region
assert "memcpy(comment_text + current_length," in region
assert "comment_text[current_length + incoming_length] = '\\n';" in region
assert "comment_text[required - 1] = '\\0';" in region
assert "strcat (comment_text" not in region
print("UIL comment-buffer growth guard present")
