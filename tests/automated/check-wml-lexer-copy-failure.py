#!/usr/bin/env python3
"""Check the WML token lexer rejects a failed dynamic copy safely."""

from pathlib import Path


source = (Path(__file__).parents[2] / "tools" / "wml" / "wmluiltok.l").read_text(
    encoding="utf-8"
)

helper_start = source.index("char *\ncopy_token_text")
helper_end = source.index("void\nadd_token", helper_start)
helper = source[helper_start:helper_end]
assert "copy = (char *) malloc (length + 1);" in helper
assert "if (copy == NULL)" in helper
assert "exit (EXIT_FAILURE);" in helper
assert "memcpy (copy, text, length + 1);" in helper

action_start = source.index("[a-zA-Z0-9_]+")
action_end = source.index(".\t\t{", action_start)
action = source[action_start:action_end]
assert action.count("copy_token_text ((XmConst char *)yytext)") == 4
assert "strcpy ((char *)" not in action
assert "malloc (strlen" not in action

print("PASS WML lexer token copies reject allocation failure")
