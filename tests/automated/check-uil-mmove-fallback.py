#!/usr/bin/env python3
"""Check the no-memmove UIL fallback handles overlap without allocation."""

import os
from pathlib import Path
import shutil
import subprocess
import tempfile


source = (Path(__file__).parents[2] / "clients" / "uil" / "UilMain.c").read_text(
    encoding="utf-8"
)
start = source.index("char *uil_mmove(char *s1, char *s2, int n)")
end = source.index("\n}\n#endif  /* NO_MEMMOVE */", start) + 2
function = source[start:end]

assert "if (n <= 0 || dst == src)" in function
assert "if (dst < src)" in function
assert "*dst++ = *src++;" in function
assert "*--dst = *--src;" in function
assert "malloc" not in function
assert "memcpy" not in function
assert "exit" not in function

compiler = shutil.which(os.environ.get("CC", "cc"))
assert compiler is not None, "a C compiler is required for the UIL move regression"
fixture = f"""
    #include <assert.h>
    #include <string.h>
    {function}
    int main(void)
    {{
        char forward[8] = "abcdef";
        char backward[8] = "abcdef";
        char unchanged[8] = "abcdef";

        assert(uil_mmove(forward, forward + 2, 4) == forward);
        assert(memcmp(forward, "cdefef", 7) == 0);
        assert(uil_mmove(backward + 2, backward, 4) == backward + 2);
        assert(memcmp(backward, "ababcd", 7) == 0);
        assert(uil_mmove(unchanged, unchanged, 6) == unchanged);
        assert(uil_mmove(unchanged, unchanged + 1, 0) == unchanged);
        assert(uil_mmove(unchanged, unchanged + 1, -1) == unchanged);
        assert(memcmp(unchanged, "abcdef", 7) == 0);
        return 0;
    }}
"""
with tempfile.TemporaryDirectory(dir=os.environ.get("TMPDIR")) as temporary:
    root = Path(temporary)
    source_file = root / "uil-mmove-fallback.c"
    executable = root / "uil-mmove-fallback"
    source_file.write_text(fixture, encoding="utf-8")
    subprocess.run(
        [compiler, "-std=c99", "-Wall", "-Wextra", "-Werror", str(source_file),
         "-o", str(executable)],
        check=True,
    )
    subprocess.run([str(executable)], check=True)

print("UIL no-memmove fallback copies overlap without allocation")
