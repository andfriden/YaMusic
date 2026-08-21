cmake_minimum_required(VERSION 3.16)

project(YaMusic VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Network)

qt_standard_project_setup(REQUIRES 6.10)

# Content of downloaded design from Figma to Qt or similar
if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/importedcontent/CMakeLists.txt")
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/importedcontent)
endif()

qt_add_executable(appYaMusic
    main.cpp

    src/Core/AppController.h
    src/Core/AppController.cpp

    src/Models/Track.h
    src/Models/Track.cpp

    src/Models/Account.h
    src/Models/Account.cpp

    src/Yandex/YandexClient.h
    src/Yandex/YandexClient.cpp

    src/Yandex/AccountParser.h
    src/Yandex/AccountParser.cpp

    src/Yandex/Auth/YandexAuth.h
    src/Yandex/Auth/YandexAuth.cpp

    src/Yandex/Account/AccountService.h
    src/Yandex/Account/AccountService.cpp

    src/Yandex/Catalog/SearchResult.h
    src/Yandex/Catalog/SearchResult.cpp

    src/Yandex/Catalog/SearchParser.h
    src/Yandex/Catalog/SearchParser.cpp

    src/Yandex/Catalog/SearchService.h
    src/Yandex/Catalog/SearchService.cpp

    src/Yandex/Catalog/SearchModel.h
    src/Yandex/Catalog/SearchModel.cpp

    src/Yandex/Catalog/CoverImageProvider.h
    src/Yandex/Catalog/CoverImageProvider.cpp
)

qt_add_qml_module(appYaMusic
    URI YaMusic
    QML_FILES
        Main.qml
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appYaMusic PROPERTIES
    # MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appYaMusic
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appYaMusic
    PRIVATE
        Qt6::Quick
        Qt6::Network
)

install(TARGETS appYaMusic
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
