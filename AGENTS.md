# Motif fork engineering and publication

Fix defects in the toolkit layer that owns the behavior. Use durable solutions,
update affected callers and contracts, and avoid workarounds or suppressed tests.
Keep source, tests, documentation and commit messages independently useful to
public toolkit consumers; never disclose private application requirements.

## Automated acceptance and publication

- A bug fix is ready for publication after relevant automated regression tests
  pass: use unit tests or native integration tests as appropriate to the defect.
  Record the tested revision or source fingerprint, commands and results.
  A successful build alone does not establish that a bug is fixed. Do not claim
  approval for unfinished changes, failing tests or unverified behavior.
- Document every accepted bug fix in `CHANGES`. Update relevant pages in `docs/`
  for changed behavior, APIs, ownership, ABI requirements or validation guidance.
  Document additions and intentional behavior changes there as well.
- Commit accepted fixes with their tests and documentation, then push them to
  both existing publication targets: rpi4 (`ckgit`) and GitHub (`origin`).
  Verify both remote branch tips contain the intended commit. Do not force push.
- Toolkit commits and pushes proceed independently of consuming applications'
  acceptance gates, development phases and selected dependency pins. No further
  human approval is required once the relevant automated tests pass. An
  application's later adoption or qualification is a separate operation.
- Keep unfinished or failing changes out of accepted-fix commits. Report actual
  validation limits accurately; never bypass a relevant failing test to publish.

Follow the parent workspace's temporary-directory and build-freshness rules.
Build rendered documentation once at feature completion from the primary checkout;
subagents must not build the documentation.
