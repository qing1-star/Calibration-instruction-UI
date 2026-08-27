cmake_minimum_required(VERSION 3.20)

set(${PACKAGE_NAME}_PublicComponents
    CalibrationTranslationWorkbench
)
set(${PACKAGE_NAME}_CompatibilityComponents)
set(${PACKAGE_NAME}_ExportComponents ${${PACKAGE_NAME}_PublicComponents})
set(${PACKAGE_NAME}_Components CalibrationTranslationWorkbench)

foreach(TARGET_NAME ${${PACKAGE_NAME}_Components})
    include(${CMAKE_CURRENT_LIST_DIR}/${TARGET_NAME}/TargetConfigSetting.cmake)
    list(APPEND ${PACKAGE_NAME}_RequiredLibsPublic ${${TARGET_NAME}_RequiredLibsPublic})
    list(APPEND ${PACKAGE_NAME}_RequiredLibsPrivate ${${TARGET_NAME}_RequiredLibsPrivate})
endforeach()

list(REMOVE_DUPLICATES ${PACKAGE_NAME}_RequiredLibsPublic)
list(REMOVE_DUPLICATES ${PACKAGE_NAME}_RequiredLibsPrivate)
list(APPEND ${PACKAGE_NAME}_RequiredLibs
    ${${PACKAGE_NAME}_RequiredLibsPublic}
    ${${PACKAGE_NAME}_RequiredLibsPrivate})
list(REMOVE_DUPLICATES ${PACKAGE_NAME}_RequiredLibs)
resolve_dependencies(${PACKAGE_NAME}_RequiredLibs)

