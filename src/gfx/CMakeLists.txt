SET(SRCS
    application.cpp
    application.h
    data.cpp
    FileBrowser.cpp
    FileBrowser.h
    input.cpp
    input.h
    MainLayer.cpp
)

target_sources(mnes PRIVATE ${SRCS})
