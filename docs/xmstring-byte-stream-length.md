---
nav_exclude: true
---
# XmString byte-stream length validation

`XmStringByteStreamLength` checks the three-byte compound-string ASN.1 identifier
before reading length bytes. Null input and a mismatched identifier return zero.
The API has no buffer-length argument; importers must establish that the encoded
header is available before calling it.

The returned size includes the header actually encoded in the input. Previously
the function recomputed the shortest header for the payload length, undercounting
long-form headers with payloads of 0–127 bytes by two. Property importers could
then advance to the wrong position or accept an outer length check that was too
small. Canonical short and long encodings retain their previous sizes; encoded
long headers for small payloads now return the correct size.

The regression check is
`tests/automated/check-xmstring-byte-stream-length.py`.

The executable production-function regression covers 262 cases: both header forms
for all lengths 0–127, long-form lengths 128/255/256/65535, invalid identifier and
null input. It also verifies process-lock balance under ASan/UBSan and records
source hashes, commands, timings and results in `receipt.json`:

```sh
python3 tests/automated/run-xmstring-stream-length-unit.py --out /absolute/fresh/results
```
