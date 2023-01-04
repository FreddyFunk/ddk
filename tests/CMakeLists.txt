# implementation is based on https://cliutils.gitlab.io/modern-cmake/chapters/testing/googletest.html

add_subdirectory("${PROJECT_SOURCE_DIR}/externals/googletest" "externals/googletest")

mark_as_advanced(
    BUILD_GMOCK BUILD_GTEST BUILD_SHARED_LIBS
    gmock_build_tests gtest_build_samples gtest_build_tests
    gtest_disable_pthreads gtest_force_shared_crt gtest_hide_internal_symbols
)

set_target_properties(gtest PROPERTIES FOLDER externals)
set_target_properties(gtest_main PROPERTIES FOLDER externals)
set_target_properties(gmock PROPERTIES FOLDER externals)
set_target_properties(gmock_main PROPERTIES FOLDER externals)

macro(package_add_test_with_libraries TESTNAME FILES LIBRARIES)
    add_executable(${TESTNAME} ${FILES} test_data.hpp)
    # do NOT link against gtest_main due to googletest issue: https://github.com/google/googletest/issues/2157
    target_link_libraries(${TESTNAME} PRIVATE gtest gmock ${LIBRARIES})
    if (WIN32)
        # fix .dll path for googletest dll files
        add_custom_command(
            TARGET ${TESTNAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_RUNTIME_DLLS:${TESTNAME}> $<TARGET_FILE_DIR:${TESTNAME}>
            COMMAND_EXPAND_LISTS
        )
    endif ()
    gtest_discover_tests(${TESTNAME}
        # set a working directory so your project root so that you can find test data via paths relative to the project root
        WORKING_DIRECTORY ${PROJECT_DIR}
        PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_DIR}"
    )
    set_target_properties(${TESTNAME} PROPERTIES FOLDER tests)
endmacro()

package_add_test_with_libraries(fsitem_test fsitem_test.cpp file_system)
package_add_test_with_libraries(fsitem_test_file_system_structures fsitem_test_file_system_structures.cpp file_system)
