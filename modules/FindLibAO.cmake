SET(LIBAO_FOUND FALSE)

Find_Path(LIBAO_INCLUDE_DIR
  ao.h
  /usr/include/ao /usr/local/include/ao
  )

Find_Library(LIBAO_LIBRARY
  ao
  /usr/lib /usr/local/lib
  )

IF(LIBAO_INCLUDE_DIR AND LIBAO_LIBRARY)
  SET(LIBAO_FOUND TRUE)
  MESSAGE(STATUS "Found libao")
ENDIF(LIBAO_INCLUDE_DIR AND LIBAO_LIBRARY)

