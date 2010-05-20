
if(DBUS_INCLUDE_DIR AND DBUS_LIBRARY AND DBUS_ARCH_INCLUDE_DIR)
	# Already in cache, be silent
	set(DBUS_FIND_QUIETLY TRUE)	
endif(DBUS_INCLUDE_DIR AND DBUS_LIBRARY AND DBUS_ARCH_INCLUDE_DIR)

set(DBUS_LIBRARY)
set(DBUS_INCLUDE_DIR)
set(DBUS_ARCH_INCLUDE_DIR)

FIND_PATH(DBUS_INCLUDE_DIR dbus/dbus.h
	/usr/include
	/usr/include/dbus-1.0
	/usr/local/include
)

FIND_PATH(DBUS_ARCH_INCLUDE_DIR dbus/dbus-arch-deps.h
	/usr/lib/include
	/usr/lib/dbus-1.0/include
  /usr/lib64/include
  /usr/lib64/dbus-1.0/include
)

FIND_LIBRARY(DBUS_LIBRARY NAMES dbus-1 dbus
	PATHS
	/usr/lib
	/usr/local/lib
)

if(DBUS_INCLUDE_DIR AND DBUS_LIBRARY AND DBUS_ARCH_INCLUDE_DIR)
   MESSAGE( STATUS "dbus found: includes in ${DBUS_INCLUDE_DIR}, library in ${DBUS_LIBRARY}")
   set(DBUS_FOUND TRUE)
else(DBUS_INCLUDE_DIR AND DBUS_LIBRARY AND DBUS_ARCH_INCLUDE_DIR)
   MESSAGE( STATUS "dbus not found")
endif(DBUS_INCLUDE_DIR AND DBUS_LIBRARY AND DBUS_ARCH_INCLUDE_DIR)

MARK_AS_ADVANCED(DBUS_INCLUDE_DIR DBUS_LIBRARY DBUS_ARCH_INCLUDE_DIR)