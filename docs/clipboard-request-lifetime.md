# Deferred clipboard request lifetime

Deferred clipboard callbacks now copy the snapshot distinguisher while holding
the process lock and use a local final-callback flag after releasing it. This
keeps callbacks independent of the shared request record while other target
callbacks run and prevents the setup path from freeing a request that has
already been published. Requests with no successfully registered data-id are
still released immediately.

Regression coverage: `tests/automated/check-clipboard-request-lifetime.py`.
