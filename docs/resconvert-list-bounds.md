# Resource converters for lists: tab units and token reader

## Unit names of a tab list

`GetNextTab` copies the unit name of a tab (`"1 inches"`) into a caller buffer
of 12 bytes. It accepted names of up to 12 characters, so a 12 character name
wrote its terminating NUL one byte past the buffer. The function now takes the
size of the buffer and rejects a name that does not leave room for the NUL. The
longest unit that the toolkit knows, `millimeters`, has 11 characters, so no
valid tab list is affected; a longer name was never a valid unit.

## Token reader

`GetNextToken` splits a resource string at a delimiter (for example the commas of
an atom list), skipping delimiters and white space that are escaped with a
backslash. It returned a pointer to the last character of the token, which cannot
distinguish an empty token from a token of one character. The consequences were:

- a delimiter at the start of the string made the reader look one byte before the
  string,
- a token of one character was returned as an empty token, and the last token was
  lost when it had one character,
- an empty token between two delimiters was returned without advancing, so a
  caller that reads until `NULL` did not terminate, and
- a backslash at the end of a token made the copy loop continue past the token.

The token is now the range from its first character up to, but excluding, its
end, and the context always advances past the delimiter. A token that consists of
white space only, or is empty, is returned as an empty string unless it is the
rest of the string, which ends the list. The only caller is the atom list
converter.

## Test

```text
python3 tests/automated/run-resconvert-list-bounds.py RESULTS
python3 tests/automated/run-resconvert-list-bounds.py RESULTS2 --source-revision HEAD
```

`check-resconvert-list-bounds.c` includes `ResConvert.c` to reach its static
functions and runs under AddressSanitizer and UndefinedBehaviorSanitizer with the
production `lib/Xm` sources; every string is held in a heap block of exactly its
length. The cases check valid tab lists and tokens (including escapes and
white space) and the strings that the fuzzer found. The crash inputs are kept as
seeds in `tests/fuzz/corpus/xm-converters`.
