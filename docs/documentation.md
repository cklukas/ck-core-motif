# Documentation builds and publication

The site builds only the focused Markdown tree under `docs/`, with explicit
navigation in `ckdocs.yml`. It does not crawl the historical `doc/` manual tree.
Both CI hosts use the same `scripts/build-docs.py` entry point and the same
source-pinned documentation compiler:

- Repository: [ck-git-hosting](https://github.com/cklukas/ck-git-hosting)
- Commit: `5019729cd37c06ae4e0c1d2c07e4127f3a72394d`
- Version: `0.8.1`

The host's installed `ckdocs` is not used. The script verifies a clean exact
source checkout, builds only the `ckdocs` target into a fresh explicit work
directory, then invokes a strict site build. No runtime tool download or package
installation occurs inside the shared script. A compiler, GNU make, Git and
Python 3 are prerequisites. The pinned source includes its diagram renderer.

## Local build

Select an explicit fresh absolute scratch path and a clean checkout of the
pinned tool source. On macOS use a mounted `/Volumes/S8T/tmp` or
`/Volumes/PRO-BLADE/tmp` scratch directory.

```sh
python3 scripts/build-docs.py \
  --tool-source "$tool_source" \
  --work "$work" \
  --out "$site" \
  --site-url https://cklukas.github.io/ck-core-motif/
```

The script refuses pre-existing work/output paths, avoiding stale outputs.
It sets `TMPDIR` beneath the selected work path. Its `receipt.json` records
source fingerprints, pinned tool tree/binary, commands, timestamps, elapsed
time and output hashes. Inspect the logs and input/output identities if a build
duration differs unexpectedly from comparable prior receipts; no duration
baseline is invented. Keep durable receipts and remove superseded scratch builds
when they are no longer in use.

The strict generator checks internal links and fragments. The shared script
also verifies expected rendered content and refuses a successful command with
no HTML. Editing source and producing verified HTML are separate events.

## Hosted CI

`.ckgit/ci.yml` requests a read-only sister named `ck-git-hosting` at the exact
pinned commit. The runner must host that sister and resolve the commit, expose
`CKGIT_SISTER_CK_GIT_HOSTING`, supply compiler prerequisites and a writable
`TMPDIR`, and treat `master` as this repository's default branch. A successful
run publishes `public` through the configured Pages mechanism and retains a
documentation evidence artifact. The intended LAN URL is
[the toolkit site](http://rpi4:8421/ck-core-motif/).

The GitHub workflow separately checks out the same tool commit and invokes the
same script. Pull requests perform strict validation without uploading or
deploying Pages. Only a successful `master` run can deploy. Repository settings
must enable GitHub Actions Pages, and the `github-pages` environment must permit
that branch. The intended public URL is
[CK Core Motif documentation](https://cklukas.github.io/ck-core-motif/).

These workflow files describe intended automation. They do not prove a hosted
run, publication, reachable URL or repository configuration. Publication is
verified only after inspecting the actual corresponding CI run and served site.
