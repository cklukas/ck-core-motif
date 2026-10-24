# UIL debug listing output

The debug build of `uil` writes symbol and machine-code details through a
small line accumulator.  The formatter now measures each variadic message,
formats it into owned storage, and emits it in bounded chunks.  Long comments,
symbol names, and other generated text therefore cannot overwrite adjacent
listing state.

Run the regression against a debug-enabled binary with:

```sh
python3 tests/automated/check-uil-listing.py /absolute/path/to/uil /absolute/path/to/results
```

The normal release build keeps the historical listing format and command-line
options; this change only makes the debug output path bounded.

Comment preservation also grows its accumulator to the complete incoming line
plus the newline and terminator. A single comment longer than the old 2000-byte
growth increment is therefore retained without an intermediate under-sized
allocation; the regression uses a 14-kilobyte comment.

Listing filenames longer than the legacy FCB storage are rejected before the
file is opened, and the per-file summary uses bounded formatting so a valid
source filename cannot overwrite listing state.

Source and include path assembly uses checked joins and rejects paths that do
not fit the legacy file-control record. Empty `-I` entries are skipped safely;
they no longer index before the beginning of the directory string.

The source-line prefix is also formatted with its destination size before the
source text is appended, keeping unusual line/file values within the existing
listing buffer.
