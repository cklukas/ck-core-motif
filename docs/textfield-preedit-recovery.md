# TextField inline preedit recovery

When overstrike preedit recovery converts the saved wide-character tail back
to the locale encoding, the conversion buffer is now retained separately from
the cursor used to scan past completed multibyte characters.  This prevents an
invalid interior-pointer free during composition updates.  Invalid conversion
or multibyte boundaries stop recovery cleanly while preserving the active
preedit buffer and widget ABI.

The TextField source is compiled as part of the native library build.  The
portable preedit-state regression remains the smoke check for composition
state ownership on builds that provide the optional font backends.
