cmake_minimum_required(VERSION 2.8.5)

project(Xarcade2Jstick)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -O3")

add_subdirectory(src)

set(EXECUTABLE_NAME "xarcade2jstick")

add_executable(${EXECUTABLE_NAME} "src/main.c")
target_link_libraries(${EXECUTABLE_NAME} xarcade2jstick-lib)

install(FILES ${CMAKE_BINARY_DIR}/${EXECUTABLE_NAME} DESTINATION /usr/local/bin
        PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
        GROUP_EXECUTE GROUP_READ
        )

add_custom_target(uninstall
        "${CMAKE_COMMAND}" -P "${CMAKE_SOURCE_DIR}/scripts/uninstall.cmake"
        )

add_custom_target(installservice
        COMMAND install -d $(DESTDIR)/lib/udev/rules.d/
        COMMAND install -m 0644 80-xarcade.rules $(DESTDIR)/lib/udev/rules.d/
        COMMAND install -d $(DESTDIR)/lib/systemd/system/
        COMMAND install -m 0644 xarcade2jstick.service $(DESTDIR)/lib/systemd/system/
        COMMAND udevadm trigger --action=add --subsystem-match=input
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/scripts
        COMMENT "Installing service."
        )

add_custom_target(uninstallservice
        COMMAND rm $(DESTDIR)/lib/udev/rules.d/80-xarcade.rules
        COMMAND rm $(DESTDIR)/lib/systemd/system/xarcade2jstick.service
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/../scripts
        COMMENT "Uninstalling service."
        )
