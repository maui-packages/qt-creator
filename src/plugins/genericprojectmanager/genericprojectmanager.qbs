import qbs 1.0
import qbs.FileInfo

import QtcPlugin

QtcPlugin {
    name: "GenericProjectManager"

    Depends { name: "Qt.widgets" }
    Depends { name: "Utils" }

    Depends { name: "Core" }
    Depends { name: "CppTools" }
    Depends { name: "TextEditor" }
    Depends { name: "ProjectExplorer" }
    Depends { name: "QtSupport" }

    files: [
        "filesselectionwizardpage.cpp",
        "filesselectionwizardpage.h",
        "genericbuildconfiguration.cpp",
        "genericbuildconfiguration.h",
        "genericmakestep.cpp",
        "genericmakestep.h",
        "genericmakestep.ui",
        "genericproject.cpp",
        "genericproject.h",
        "genericproject.qrc",
        "genericprojectconstants.h",
        "genericprojectfileseditor.cpp",
        "genericprojectfileseditor.h",
        "genericprojectmanager.cpp",
        "genericprojectmanager.h",
        "genericprojectnodes.cpp",
        "genericprojectnodes.h",
        "genericprojectplugin.cpp",
        "genericprojectplugin.h",
        "genericprojectwizard.cpp",
        "genericprojectwizard.h",
        "pkgconfigtool.cpp",
        "pkgconfigtool.h",
    ]

    Group {
        name: "Tests"
        condition: project.testsEnabled
        files: [
            "cppmodelmanagerhelper.cpp", "cppmodelmanagerhelper.h",
            "genericprojectplugin_test.cpp",
        ]

        cpp.defines: outer.concat(['SRCDIR="' + FileInfo.path(filePath) + '"'])
    }
}
