cmake_minimum_required(VERSION 2.6 FATAL_ERROR )

# must be done BEFORE project in order to work!
include(cmake/install_prefix.cmake)

project(strusPrototypeModuleV1)
set( STRUS_PROTOTYPE_MODULE_V1_MAJOR_VERSION 0 )
set( STRUS_PROTOTYPE_MODULE_V1_MINOR_VERSION 0 )
set( STRUS_PROTOTYPE_MODULE_V1_PATCH_VERSION 3 )
set( STRUS_PROTOTYPE_MODULE_V1_VERSION ${STRUS_PROTOTYPE_MODULE_V1_MAJOR_VERSION}.${STRUS_PROTOTYPE_MODULE_V1_MINOR_VERSION}.${STRUS_PROTOTYPE_MODULE_V1_PATCH_VERSION} )

include( cmake/dist.cmake )
include( cmake/LinuxDistribution.cmake )
include( cmake/build_rules.cmake )
include( cmake/boost.cmake)
include( cmake/intl.cmake )
include( cmake/find_strus_package.cmake )

find_strus_package( core )
find_strus_package( analyzer )
find_strus_package( module )

configure_file( "${PROJECT_SOURCE_DIR}/dist/obs/deploy_to_obs.sh.in"  "${PROJECT_SOURCE_DIR}/dist/obs/deploy_to_obs.sh"  @ONLY )
configure_file( "${PROJECT_SOURCE_DIR}/dist/archlinux/PKGBUILD.in"  "${PROJECT_SOURCE_DIR}/dist/archlinux/PKGBUILD"  @ONLY )
configure_file( "${PROJECT_SOURCE_DIR}/dist/obs/strusprototypemodulev1.dsc.in"  "${PROJECT_SOURCE_DIR}/dist/obs/strusprototypemodulev1.dsc"  @ONLY )
configure_file( "${PROJECT_SOURCE_DIR}/dist/redhat/strusprototypemodulev1.spec.in"  "${PROJECT_SOURCE_DIR}/dist/redhat/strusprototypemodulev1.spec"  @ONLY )

set( CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" )
set( CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON )

enable_testing()

# Subdirectories:
add_subdirectory( src )
  
include( cmake/report_build_settings.cmake )

include( cmake/standard_targets.cmake )

