#!/usr/bin/env python3
"""
patch_eflags.py — normalize the fbb prebuilt libyaffs2.a for the hi3322 build.

The fbb archive was compiled with -mabi=ilp32d (ELF e_flags = 0x405:
RVC | double-float ABI | vendor bit), while this project links everything
with -march=rv32imc -mabi=ilp32 (e_flags = 0x1). lld refuses to mix the two
("cannot link object files with different floating-point ABI").

The archive contains ZERO floating-point instructions (verified by full
instruction census: no f*.s/f*.d/f*.w opcodes), so rewriting the float-ABI
bits is semantically inert. Zcmp/Zcb/Zbb instructions present in the code
(e.g. c.push/c.pop, rev8) are executable by the hi3322 core — the fbb
production firmware runs this exact archive on the same silicon — and are
unaffected by e_flags.

What this script does (idempotent):
  1. extracts every member of the source archive (preserving order);
  2. rewrites the ELF32 e_flags field (header offset 0x24) from 0x405 to
     0x1 in each member;
  3. re-archives the members under the same names, in the same order;
  4. verifies the global text-symbol count is unchanged.

Usage:
  python3 patch_eflags.py <src.a> <dst.a> <llvm-ar>

Evidence and background: YAFFS2_BINARY_LIB_PORTING_PLAN.md section 3.1.
"""

import os
import shutil
import struct
import subprocess
import sys
import tempfile

SRC_FLAGS = 0x405   # RVC | double-float ABI | vendor bit (fbb build)
DST_FLAGS = 0x1     # RVC | soft-float ABI   (this project)

ELF_MAGIC = b"\x7fELF"
ELFCLASS32 = 1
E_FLAGS_OFF = 0x24


def patch_object(path):
    with open(path, "rb") as fh:
        data = bytearray(fh.read())
    if len(data) < 0x28 or data[:4] != ELF_MAGIC or data[4] != ELFCLASS32:
        return "skip(not-elf32)"
    flags = struct.unpack_from("<I", data, E_FLAGS_OFF)[0]
    if flags == DST_FLAGS:
        return "ok(already-0x1)"
    if flags != SRC_FLAGS:
        return "skip(unexpected-flags-0x%x)" % flags
    struct.pack_into("<I", data, E_FLAGS_OFF, DST_FLAGS)
    with open(path, "wb") as fh:
        fh.write(data)
    return "patched(0x405->0x1)"


def text_symbol_count(llvm_ar, archive):
    # use llvm-nm next to llvm-ar
    nm = os.path.join(os.path.dirname(llvm_ar), "llvm-nm")
    res = subprocess.run([nm, "--defined-only", archive],
                         capture_output=True, text=True, check=True)
    syms = set()
    for line in res.stdout.splitlines():
        parts = line.split()
        if len(parts) == 3 and parts[1] == "T":
            syms.add(parts[2])
    return len(syms)


def main():
    if len(sys.argv) != 4:
        sys.stderr.write(__doc__)
        return 2
    src, dst, ar = sys.argv[1:4]
    src = os.path.abspath(src)
    dst = os.path.abspath(dst)
    ar = os.path.abspath(ar)
    if not os.path.isfile(src):
        sys.stderr.write("missing source archive: %s\n" % src)
        return 1

    before = text_symbol_count(ar, src)

    tmp = tempfile.mkdtemp(prefix="yaffs_eflags.")
    try:
        # member list in archive order
        listing = subprocess.run([ar, "t", src], capture_output=True,
                                 text=True, check=True)
        members = [m for m in listing.stdout.split("\n") if m.strip()]
        if not members:
            sys.stderr.write("empty archive: %s\n" % src)
            return 1

        # extract (flat: member names are unique file names)
        subprocess.run([ar, "x", src], cwd=tmp, check=True)

        counts = {}
        for m in members:
            res = patch_object(os.path.join(tmp, m))
            counts[res] = counts.get(res, 0) + 1

        # re-archive in the original order
        subprocess.run([ar, "rcs", os.path.abspath(dst)] + members,
                       cwd=tmp, check=True)
    finally:
        shutil.rmtree(tmp, ignore_errors=True)

    after = text_symbol_count(ar, dst)
    if before != after:
        sys.stderr.write("FATAL: symbol count changed %d -> %d\n"
                         % (before, after))
        return 1

    for k in sorted(counts):
        print("  %-28s %d members" % (k, counts[k]))
    print("  text symbols: %d (unchanged)" % before)
    print("  wrote %s" % dst)
    return 0


if __name__ == "__main__":
    sys.exit(main())
