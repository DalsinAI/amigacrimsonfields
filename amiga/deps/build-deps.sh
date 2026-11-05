#!/bin/sh
# Builds zlib 1.3.1, which Crimson Fields reads its compressed data and
# saved games with and the stove lacks, static, for AmigaOS 3.x on a
# 68040 + FPU.
#
#   amiga/deps/build-deps.sh STOVE OUT [CACHE]
#
# The source tarballs are fetched once into CACHE (default amiga/deps/cache,
# not kept in git) and checked against the SHA-256 sums below. OUT gets
# include/ and lib/.
# Copyright (C) 2026 AmiCradle Team. GPL 2 or later, like Crimson Fields.
set -e

STOVE=$1
OUT=$2
HERE=$(cd "$(dirname "$0")" && pwd)
CACHE=${3:-$HERE/cache}
[ -n "$STOVE" ] && [ -n "$OUT" ] || { echo "usage: $0 STOVE OUT [CACHE]"; exit 1; }
mkdir -p "$CACHE" "$OUT/include" "$OUT/lib" "$OUT/src"
OUT=$(cd "$OUT" && pwd)

CC="$STOVE/bin/m68k-amigaos-gcc"
AR="$STOVE/bin/m68k-amigaos-ar"
RANLIB="$STOVE/bin/m68k-amigaos-ranlib"
CFLAGS="-noixemul -m68040 -m68881 -O2 -fomit-frame-pointer -fno-tree-loop-distribute-patterns"

fetch() { # url file sha256
	if [ ! -f "$CACHE/$2" ]; then
		curl -fsSL --max-time 300 -o "$CACHE/$2.part" "$1"
		mv "$CACHE/$2.part" "$CACHE/$2"
	fi
	echo "$3  $CACHE/$2" | sha256sum -c - >/dev/null || { echo "$2: wrong SHA-256"; exit 1; }
}

fetch https://zlib.net/fossils/zlib-1.3.1.tar.gz zlib-1.3.1.tar.gz \
	9a93b2b7dfdac77ceba5a558a580e74667dd6fede4585b91eefb60f03b72df23

cd "$OUT/src"

# zlib: its sources as they are.
if [ ! -f "$OUT/lib/libz.a" ]; then
	rm -rf zlib-1.3.1 && tar xzf "$CACHE/zlib-1.3.1.tar.gz"
	cd zlib-1.3.1
	for f in adler32 crc32 deflate infback inffast inflate inftrees trees zutil \
	         compress uncompr gzclose gzlib gzread gzwrite; do
		$CC $CFLAGS -DHAVE_UNISTD_H -c -o $f.o $f.c
	done
	$AR rc "$OUT/lib/libz.a" *.o && $RANLIB "$OUT/lib/libz.a"
	cp zlib.h zconf.h "$OUT/include/"
	cd ..
fi

echo "deps in $OUT"
