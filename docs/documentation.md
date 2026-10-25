# Documentation builds and publication

The site builds only the focused Markdown tree under `docs/`, with explicit
navigation in `ckdocs.yml`. It does not crawl the historical `doc/` manual tree.
The homepage uses ckdocs' built-in `layout: home` and `hero` metadata to place
the introduction beside the artwork, with the image on the right on wide
screens and a stacked layout on narrow screens. This is supported by the
pinned compiler below and needs no custom stylesheet.
Both CI hosts use the same `scripts/build-docs.py` entry point and the same
source-pinned documentation compiler:

- Repository: [ck-git-hosting](https://github.com/cklukas/ck-git-hosting)
- Commit: `5019729cd37c06ae4e0c1d2c07e4127f3a72394d`
- Version: `0.8.1`

The host's installed `ckdocs` is not used. The script verifies a clean exact
source checkout or verified export archive, builds only the `ckdocs` target into a fresh explicit work
directory, then invokes the site build. No runtime tool download or package
installation occurs inside the shared script. A compiler, GNU make, Git and
Python 3 are prerequisites. The pinned source includes its diagram renderer.

## Checkout and archive provenance

Git tool checkouts must have the exact pinned commit/tree and no tracked or
untracked changes; `local.mk` overrides are refused even if ignored by Git.
A tool source archive must carry the exact substituted `.ckgit/build-commit`
stamp. The script then reconstructs its complete Git tree object identity,
including paths, modes and contents, and compares it with the pinned tree
`4105740b7e806ecf3eba8ae5c739fc8bafbd9122`. Only the verified export stamp is
normalized to its original template bytes for that comparison. Extra, missing
or changed files fail verification. A stamp by itself is insufficient for the
tool source.

This repository also exports its commit through `.gitattributes` and
`.ckgit/build-commit`. A Git checkout records its commit and dirty state. A
repository archive requires that export stamp or the hosting runner's explicit
`CKGIT_CI=1` / `CKGIT_COMMIT` identity; if both exist they must agree. Git dirty
state is unavailable in that case and is recorded as such, alongside the exact
documentation input fingerprints. Unknown or conflicting source identities are
refused. The tool archive pin and repository archive provenance are distinct:
the runner's repository commit is never mistaken for the sister tool's commit.

The hosting 0.7.1 runner creates Git checkouts by default for both the project
and sisters, exports the project commit, and removes `.git` only for an explicit
archive checkout mode. The workflow does not require that mode. The export
path additionally supports genuine `git archive` source deliveries.

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
time and output hashes. Documentation input fingerprints cover both Markdown
pages and image assets, including the homepage hero, header logo and favicon.
Inspect the logs and input/output identities if a build
duration differs unexpectedly from comparable prior receipts; no duration
baseline is invented. Keep durable receipts and remove superseded scratch builds
when they are no longer in use.

The generator checks internal links and fragments while retaining the
historical detail pages as searchable, unlisted content. The shared script
also verifies expected rendered content and refuses a successful command with
no HTML. Editing source and producing verified HTML are separate events.

## Native component gallery

The native component gallery is published as part of this documentation site:

- [Open the component gallery](modern-gallery.md)
- [Browse the manifest-backed component catalogue](modern-gallery-components.md)

The gallery page and catalogue are generated documentation targets, not links
to repository source files. The catalogue embeds the verified PNG scenarios
captured from `ckmotif-gallery`; each entry retains its manifest anchor,
interaction scenario, source-fragment identifier, and screenshot evidence.

## Hosted CI

`.ckgit/ci.yml` requests a read-only sister named `ck-git-hosting` at the exact
pinned commit. The runner must host that sister and resolve the commit, expose
`CKGIT_SISTER_CK_GIT_HOSTING`, supply compiler prerequisites and a writable
`TMPDIR`, and treat `master` as this repository's default branch. A successful
run publishes `public` through the configured Pages mechanism and retains a
documentation evidence artifact. The intended LAN URL is
[the toolkit site](http://rpi4:8421/ck-core-motif/).

Before the site build, both hosted workflows use the current
`ckmotif-gallery` into an isolated prefix and capture the `modern-light`
1.5x demo set under Xvfb. The generated PNGs and manifest are then used for
that run's site output, so a stale checked-in binary hash cannot silently be
published. The gallery startup profile check keeps legacy widgets from being
re-themed after realization, which previously caused rpi4 `BadGC` failures.

The GitHub workflow separately checks out the same tool commit and invokes the
same script. Pull requests perform strict validation without uploading or
deploying Pages. Only a successful `master` run can deploy. Repository settings
must enable GitHub Actions Pages, and the `github-pages` environment must permit
that branch. The intended public URL is
[CK Core Motif documentation](https://cklukas.github.io/ck-core-motif/).

These workflow files describe intended automation. They do not prove a hosted
run, publication, reachable URL or repository configuration. Publication is
verified only after inspecting the actual corresponding CI run and served site.
