# UIL locale charset input bounds

`sem_charset_lang_name` compares the locale charset name selected by UIL's
`LANG` handling with the compiler's known charset table. The old implementation
copied that input directly into a 200-byte stack buffer. An unusually long or
malformed locale value could therefore overwrite the compiler stack before the
lookup completed.

The lookup now rejects null and 199-byte-or-longer names before copying. This
preserves the existing behavior for every representable charset name while
making malformed environment input fail as an ordinary unknown charset.

The source regression is:

```sh
python3 tests/automated/check-uil-charset-input.py
```
