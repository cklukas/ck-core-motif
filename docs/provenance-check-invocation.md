# Provenance check invocation

The documentation and gallery provenance checks now distinguish a generic
no-argument test sweep from their real build-time invocation. Running either
script without required artifacts reports a clear `SKIP` and exits successfully;
the explicit Makefile invocations still perform the complete archive or
manifest/provenance validation.

This keeps repository-wide automated discovery useful without weakening the
checks used by documentation and gallery builds.
