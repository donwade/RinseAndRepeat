set(COMPONENT_ADD_INCLUDEDIRS
    src
    )
file(GLOB SRCS
     src/*.cpp
     )
set(COMPONENT_SRCS ${SRCS})

if (IDF_VERSION_MAJOR GREATER_EQUAL 5)
    set(COMPONENT_REQUIRES nvs_flash efuse driver esp_timer esp_lcd esp_mm)
elseif ((IDF_VERSION_MAJOR EQUAL 4) AND (IDF_VERSION_MINOR GREATER 3) OR IDF_VERSION_MAJOR GREATER 4)
    set(COMPONENT_REQUIRES nvs_flash efuse esp_lcd)
else()
    set(COMPONENT_REQUIRES nvs_flash efuse)
endif()

### If you use arduino-esp32 components, please activate next comment line.
list(APPEND COMPONENT_REQUIRES arduino-esp32)


message(STATUS "M5GFX use components = ${COMPONENT_REQUIRES}")

register_component()

