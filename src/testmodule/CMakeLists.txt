cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

# --------------------------------------
# SOURCES AND INCLUDES
# --------------------------------------
include_directories(
	"${Boost_INCLUDE_DIRS}"
	"${strus_INCLUDE_DIRS}"
	"${strusanalyzer_INCLUDE_DIRS}"
	"${strusmodule_INCLUDE_DIRS}"
	"${PROJECT_SOURCE_DIR}/include"
	"${PROJECT_SOURCE_DIR}/include/strus"
)

link_directories( 
	"${Boost_LIBRARY_DIRS}"
	"${strus_LIBRARY_DIRS}"
	"${strusanalyzer_LIBRARY_DIRS}"
	"${strusmodule_LIBRARY_DIRS}" )

# ------------------------------------
# LIBRARY
# ------------------------------------
add_library( modtest MODULE modtest.cpp internationalization.cpp postingIteratorHelpers.cpp positionWindow.cpp window_joinop.cpp operatortest.cpp weightingtest.cpp summarizertest.cpp summarizertest2.cpp formulaInterpreter.cpp weightingFormula.cpp utils.cpp weightingMetadata2.cpp )
set_target_properties( modtest PROPERTIES PREFIX "")
target_link_libraries( modtest strus_module ${Intl_LIBRARIES} ${Boost_LIBRARIES} )

# ------------------------------
# INSTALLATION
# ------------------------------
install( TARGETS modtest
           LIBRARY DESTINATION ${LIB_INSTALL_DIR}/strus/modules
	   RUNTIME DESTINATION bin )



