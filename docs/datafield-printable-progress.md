# DataField printable-text progress

DataField printable-text validation now rejects zero-length multibyte
decodings as well as negative `mblen()` results.  This guarantees that the
validation loop always advances or returns failure, preventing a hang on NUL
or malformed locale input.  Valid printable text and wide-character checks
retain their existing behavior.

The warning-enabled DataField build and Xvfb DataField regression are the
native smoke checks for this path.
