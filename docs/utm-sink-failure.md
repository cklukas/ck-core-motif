# UTM sink failure cleanup

When `XmeNamedSink` rejects a request, its package is now removed by identity
from the pending UTM queue and released. Removing the specific node preserves
other queued requests and prevents a later destination callback from consuming
stale parameters.

`tests/automated/check-utm-sink-failure.py` protects this queue lifecycle.
