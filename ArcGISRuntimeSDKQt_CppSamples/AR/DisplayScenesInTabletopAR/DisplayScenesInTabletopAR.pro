#-------------------------------------------------
# Copyright 2020 Esri.

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# http://www.apache.org/licenses/LICENSE-2.0

# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#-------------------------------------------------

mac {
    cache()
}

#-------------------------------------------------------------------------------

CONFIG += c++14

# additional modules are pulled in via arcgisruntime.pri
QT += opengl qml quick

TEMPLATE = app
TARGET = DisplayScenesInTabletopAR

ARCGIS_RUNTIME_VERSION = 100.12
include($$PWD/arcgisruntime.pri)

#-------------------------------------------------------------------------------

HEADERS += \
    DisplayScenesInTabletopAR.h \
    PermissionsHelper.h

SOURCES += \
    main.cpp \
    DisplayScenesInTabletopAR.cpp \
    PermissionsHelper.cpp

RESOURCES += DisplayScenesInTabletopAR.qrc

win32 {
    LIBS += \
        Ole32.lib
}

ios {
    INCLUDEPATH += $$PWD
    DEPENDPATH += $$PWD

    OTHER_FILES += \
        $$PWD/Info.plist

    QMAKE_INFO_PLIST = $$PWD/Info.plist
}

#-------------------------------------------------------------------------------
# AR configuration

# The path to the ArcGIS runtime toolkit for Qt sources, corresponding to the files downloaded
# from the GitHub repo: https://github.com/Esri/arcgis-runtime-toolkit-qt

# path of the toolkit relative to the sample
ARCGIS_TOOLKIT_PATH = $$PWD/../../../arcgis-runtime-toolkit-qt

exists($$ARCGIS_TOOLKIT_PATH/augmentedreality/CppApi/ArCppApi.pri) {
    include($$ARCGIS_TOOLKIT_PATH/augmentedreality/CppApi/ArCppApi.pri)
} else {
    error(ARCGIS_TOOLKIT_PATH is missing which is required to build this application.)
}

#-------------------------------------------------------------------------------

android {
    QT += androidextras
    INCLUDEPATH += $$PWD
    DEPENDPATH += $$PWD

    ANDROID_ABIS = arm64-v8a
}

contains(ANDROID_TARGET_ARCH,arm64-v8a) {
    ANDROID_PACKAGE_SOURCE_DIR = \
        $$PWD/../../../supportFiles/android
}
