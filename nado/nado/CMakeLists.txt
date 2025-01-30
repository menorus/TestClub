# CMakeList.txt: проект CMake для nado; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#

# Добавьте источник в исполняемый файл этого проекта.
add_executable (nado "nado.cpp" "nado.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET nado PROPERTY CXX_STANDARD 20)
endif()

# TODO: Добавьте тесты и целевые объекты, если это необходимо.
