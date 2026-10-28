# Text source range bounds

Legacy text-source conversion now checks `XmTextPosition` ranges before
allocating or passing positions through the `int`-indexed source reader.  A
range that cannot be represented safely is rejected with a null result; valid
ranges retain the existing byte and wide-character conversion behavior.

The source reader also checks the character-width multiplication used for its
scratch buffer before allocating it. This keeps wide-character reads from
wrapping an allocation size even when a source reports a very large block. The
contract is covered by `tests/automated/check-text-source-allocation.py`.

Run the native installed-prefix regression with:

```sh
python3 tests/automated/run-text-source-regressions.py /absolute/path/to/prefix /absolute/path/to/results
```
`ReadSource` validates its source, widget list, and returned block payload
before converting text.  A failed provider read that reports a positive length
without a payload now returns safely instead of dereferencing null, while valid
large-range reads retain their existing checked sizing behavior.

The modify-verify bridge applies the same contract to callback-provided blocks:
nonzero lengths require a non-null payload, and a callback cannot replace the
text block with a null structure.  Invalid callback data is rejected before
copying; valid edits are unchanged.

The wide-character modify-verify path applies the same validation before
`mbstowcs`/`wcstombs` conversion and rejects a null callback replacement.  This
keeps malformed client callback data from becoming a conversion crash.

The bridge also rejects incomplete caller state (widget, source, range, block,
or output pointers) before accessing it.  Existing valid verification calls are
unchanged.
