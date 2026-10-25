# x86_64 native text qualification

The bounded x86_64 text result was produced on WSL2 with the installed
toolkit prefix from the recorded x86_64 cohort:

- prefix: `/mnt/v/tmp/ckmotif-x86-20261025/prefix-r1`
- receipt: `/mnt/v/tmp/ckmotif-x86-20261025/results/text-pass-r2/receipt.json`
- fontconfig: `/mnt/v/tmp/ckmotif-x86-20261025/fonts/config/fonts.conf`
- fontconfig SHA-256: `3a514e14d4ddc49ef8a8eec8d293d551753ec374d03393925c22eb47d436f778`

The runner invocation was:

```sh
python3 tests/automated/check-native-features.py \
  --prefix /mnt/v/tmp/ckmotif-x86-20261025/prefix-r1 \
  --out /mnt/v/tmp/ckmotif-x86-20261025/results/text-pass-r2 \
  --fontconfig-file /mnt/v/tmp/ckmotif-x86-20261025/fonts/config/fonts.conf \
  --only incremental-map --only preedit --only unicode-widgets \
  --only layout --only line-context --only font-resize
```

The installed `abi`, `incremental-map`, `preedit`, `layout`, `line-context`,
`unicode-widgets` and `font-resize` fixtures passed. The task-local fallback
corpus was staged under `V:\tmp` from these packages:

| Package | Version | SHA-256 |
| --- | --- | --- |
| `fonts-noto-cjk` | `1:20220127+repack1-1` | `e804b3474a79bd70d5f03f590e2d06dbc5ce17f48be7cfaf6c48086471edd5be` |
| `fonts-noto-core` | `20201225-1build1` | `653d237d5c4e8fcebb2710129ebbd7bf9bc248d03dfff23fcf261b0372fded69` |
| `fonts-symbola` | `2.60-1.1` | `c0ded2d56a594ff290e0bd9edd8fd7ddf72b2c85af4c8e4833557c19ee414585` |
| `fonts-noto-color-emoji` | `2.047-0ubuntu0.22.04.1` | `ee5cf17c294f16f9bc1a7683fb3e372fae1179d3b69baaae4538d0e9f372eb82` |

The color-emoji face is explicitly rejected in this Xvfb configuration. With
it enabled, the `large-text` stress fixture reaches an X11 Render extension
`RenderAddGlyphs` `BadLength`; with it rejected, the monochrome fallback does
not provide complete distant supplementary caret coverage. `large-text`, real
external IME services, physical keyboard layouts and broad sanitizer ownership
therefore remain open gates. No fixture was weakened to turn either result into
a pass.
