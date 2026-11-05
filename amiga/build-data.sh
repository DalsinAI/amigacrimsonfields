#!/bin/sh
# Builds Crimson Fields' data files on the build machine (Linux), as
# upstream's tools/, gfx/, locale/ and levels/ Makefiles do: the host tools
# mkdatafile, mktileset, mkunitset, mklocale and cfed (on the host's SDL 2,
# through src/common/sdlcompat.h, and zlib), then cf.dat, default.tiles,
# default.units, locale/*.dat and levels/*.lev. The files are written
# little-endian whatever the machine, and read the same way on the Amiga.
#
#   amiga/build-data.sh OUT
#
# Needs a host C++ compiler, sdl2-config and zlib's headers.
# Copyright (C) 2026 AmiCradle Team. GPL 2 or later, like Crimson Fields.
set -e

OUT=$1
[ -n "$OUT" ] || { echo "usage: $0 OUT"; exit 1; }
TOP=$(cd "$(dirname "$0")/.." && pwd)
mkdir -p "$OUT/tools" "$OUT/locale" "$OUT/levels"
OUT=$(cd "$OUT" && pwd)
HCXX=${HCXX:-g++}
HCC=${HCC:-gcc}
SDLCFLAGS=$(sdl2-config --cflags)
SDLLIBS=$(sdl2-config --libs)
FLAGS="-O1 -include $TOP/src/common/sdlcompat.h -DDISABLE_SOUND -DHAVE_LIBZ \
 -DHAVE_DIRENT_H -DHAVE_UNISTD_H -DHAVE_STRCASECMP -DHAVE_STRNCASECMP \
 -I$TOP/src/common -I$TOP/src/comet -I$TOP/amiga/host-include $SDLCFLAGS"

C=$TOP/src/common
T=$TOP/tools
M=$TOP/src/comet

build() { # name sources...
	name=$1; shift
	objs=""
	for s in "$@"; do
		o="$OUT/tools/$(echo "$s" | sed "s|$TOP/||; s|/|_|g; s|\.c.*$|.o|")"
		case $s in
		*.c) $HCC $FLAGS -c -o "$o" "$s" ;;
		*) $HCXX $FLAGS -c -o "$o" "$s" ;;
		esac
		objs="$objs $o"
	done
	$HCXX -o "$OUT/tools/$name" $objs $SDLLIBS -lz
}

COMMON_FILE="$C/SDL_zlib.c $C/fileio.cpp $C/sdlcompat.cpp"
COMMON_SET="$COMMON_FILE $C/lang.cpp $C/lset.cpp $C/rect.cpp $C/sound.cpp $C/strutil.cpp $C/surface.cpp"

build mkdatafile $T/mkdatafile.cpp $T/mksurface.cpp $COMMON_FILE $C/surface.cpp
build mktileset $T/mktileset.cpp $T/mksurface.cpp $T/parser.cpp $COMMON_SET
build mkunitset $T/mkunitset.cpp $T/mksurface.cpp $T/parser.cpp $COMMON_SET
build mklocale $T/mklocale.cpp $COMMON_FILE $C/lang.cpp $C/strutil.cpp
build cfed $T/cfed.cpp $T/parser.cpp $COMMON_SET $C/list.cpp $C/misc.cpp \
	$M/building.cpp $M/map.cpp $M/mission.cpp $M/unit.cpp

G=$TOP/gfx
"$OUT/tools/mkdatafile" "$G/CFIcons.bmp" "$OUT/cf.dat"
"$OUT/tools/mktileset" "$T/default.tsrc" "$OUT/default.tiles" "$G"
"$OUT/tools/mkunitset" "$T/default.usrc" "$OUT/default.units" "$G"
for t in "$TOP"/locale/*.tmpl; do
	"$OUT/tools/mklocale" -l "$t" "$OUT/locale/$(basename "$t" .tmpl).dat"
done
for s in "$TOP"/levels/*.src; do
	"$OUT/tools/cfed" "$s" --tiles "$OUT/default.tiles" --units "$OUT/default.units" \
		-o "$OUT/levels/$(basename "$s" .src).lev"
done
echo "data in $OUT"
