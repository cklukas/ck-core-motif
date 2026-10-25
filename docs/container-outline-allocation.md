# Container outline allocation rollback

Container detail layout now allocates a replacement outline segment array
before releasing the current one. If allocation fails, the current outline
remains available and the layout is marked not ready for this pass instead of
discarding valid geometry.

The focused source regression is
`tests/automated/check-container-outline-allocation.py`.
