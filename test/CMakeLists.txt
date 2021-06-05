# find Check
FIND_PACKAGE(PkgConfig REQUIRED)
PKG_SEARCH_MODULE(CHECK REQUIRED check)

SET(LIBS zsr m ${CHECK_LIBRARIES})

ADD_EXECUTABLE(common_test common.c)
TARGET_LINK_LIBRARIES(common_test ${LIBS})
ADD_TEST(NAME common_test COMMAND common_test)
