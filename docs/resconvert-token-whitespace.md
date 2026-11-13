# Resource token trailing whitespace

The resource converter trims ordinary trailing whitespace from a token, but
must preserve whitespace escaped with a backslash. `GetNextToken` now checks
the byte before the trailing whitespace (`*(e - 2)`, where `e` is one past the
end of the token) rather than subtracting one from the current character
value. The latter was a precedence bug: escaped trailing spaces were trimmed as
if they were unescaped. How the token reader delimits tokens and empty tokens
is described in [resconvert-list-bounds.md](resconvert-list-bounds.md).

The behavior is covered by
`tests/automated/check-resconvert-token-whitespace.py` and the existing token
allocation regression.
