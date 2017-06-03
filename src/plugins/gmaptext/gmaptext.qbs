import qbs 1.0

TiledPlugin {
    cpp.defines: ["GMAPTEXT_LIBRARY"]

    files: [
        "gmaptext_global.h",
        "gmaptextplugin.cpp",
        "gmaptextplugin.h",
        "plugin.json",
    ]
}
