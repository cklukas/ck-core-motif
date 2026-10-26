# Resource token trailing whitespace

The resource converter trims ordinary trailing whitespace from a token, but
must preserve whitespace escaped with a backslash. `GetNextToken` now checks
the preceding byte (`*(e - 1)`) rather than subtracting one from the current
character value. The latter was a precedence bug: escaped trailing spaces were
trimmed as if they were unescaped.

The behavior is covered by
`tests/automated/check-resconvert-token-whitespace.py` and the existing token
allocation regression.
