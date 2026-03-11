import glob
import os
import textwrap

this_dir = os.path.dirname(os.path.realpath(__file__))
t89_examples_dir = os.path.join(this_dir, "examples")
t89_examples = glob.glob(os.path.join(t89_examples_dir, "*.89t"))


def hexdump(data, width=16, size=None):
    if size:
        data = data[: min(size, len(data))]
    for i in range(0, len(data), width):
        chunk = data[i : i + width]
        hexvals = " ".join(f"{b:02X}" for b in chunk)
        ascii_vals = "".join(chr(b) if 32 <= b < 127 else "." for b in chunk)
        print(f"{i:08X}  {hexvals:<48}  {ascii_vals}")


for i, fname in enumerate(t89_examples):
    with open(fname, "rb") as f:
        print("file:", fname)
        hexdump(f.read(), 16, 16 * 6)
        print()
        print("=" * 16)
        print()
