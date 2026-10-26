from pathlib import Path

source = Path("clients/mwm/WmResParse.c").read_text(encoding="utf-8")
start = source.index("#else\n\t    {\n\t\tsize_t line_length", source.index("GetNextLine"))
end = source.index("#endif", start)
region = source[start:end]

assert "line_length >= 2" in region
assert "string[line_length - 2]" in region
assert "strlen((char *)string) - 2" not in region
print("PASS MWM line-continuation parsing avoids short-line underflow")
