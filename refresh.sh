#!/bin/sh

DNLOAD="../dnload/dnload.py"
if [ ! -f "${DNLOAD}" ] ; then
  DNLOAD="../dnload/dnload.py"
  if [ ! -f "${DNLOAD}" ] ; then
    DNLOAD="/usr/local/src/dnload/dnload.py"
    if [ ! -f "${DNLOAD}" ] ; then
      DNLOAD="/usr/local/src/faemiyah-demoscene/dnload/dnload.py"
      if [ ! -f "${DNLOAD}" ] ; then
        echo "${0}: could not find dnload.py"
        exit 1
      fi
    fi
  fi
fi

if [ ! -f "src/dnload.h" ] ; then
  touch src/dnload.h
fi

python3 "${DNLOAD}" -v src/main.cpp -E --rand=gnu -DVGL_DISABLE_CEIL -DVGL_DISABLE_DEPTH_TEXTURE -DVGL_DISABLE_EDGE -DVGL_DISABLE_OGG -DVGL_DISABLE_POLYGON_OFFSET -DVGL_DISABLE_STENCIL -DVGL_USE_BONE_STATE_FULL_TRANSFORM $*
if [ $? -ne 0 ] ; then
  echo "${0}: regenerating symbols failed"
  exit 1
fi

exit 0
