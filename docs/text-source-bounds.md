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

The runner needs matching installed headers and `lib/libXm.so`. It runs
headlessly without an X server and verifies the resolved library path with
`dladdr`, rather than assuming the linker search path selected the intended
toolkit. Its receipt records source, header, library and executable hashes,
commands, timings and exit results. Source, headers and library must remain
unchanged throughout validation.

The fixture checks valid full and substring reads, valid empty narrow/wide
results, invalid negative/reversed ranges, incomplete reads beyond the source,
oversized positions on platforms with wider `XmTextPosition`, missing
widget/source and zero character width. Valid wide extraction is compared
with an independent wide-string literal. These are rejection and conversion
contracts, not a claim that multi-gigabyte documents are supported.

On 2026-11-01 the expanded fixture passed 26 checks on an isolated ARM64
ABI9 library snapshot with SHA-256
`cb73f5a93257004b4509b09184782f17a99bdf583bf299cd5765b1540a4faa7a`.
A deliberately mismatched expected library path was rejected. This bounded
native result does not imply sanitizer, x86_64 or full text-editing qualification.

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
