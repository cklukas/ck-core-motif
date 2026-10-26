---
nav_exclude: true
---
# Text search length conversion

Forward and backward Text searches now reject null or oversized search strings
before passing their byte length into the legacy `int` character-count API.
This prevents truncation from turning a large search term into an unrelated
short query.

Regression coverage: `tests/automated/check-text-find-length.py`.
