# Todo print staging and callback contracts

The Todo demo now uses a typed C23 definition for its menu-management callback.
When printing, it creates a unique temporary file beneath `TMPDIR`; if no
temporary directory is configured it uses the current directory rather than a
shared hard-coded system path.  The file is removed after the synchronous
print operation.  The print loop also skips absent pages and keeps the selected
range within the page array.

The companion database serializer also uses typed C23 declarations for its
newline escaping and unescaping helpers, so the full Todo executable builds
with the same compiler profile.

The focused source-contract check is:

```sh
python3 tests/automated/check-todo-print-staging.py
```
