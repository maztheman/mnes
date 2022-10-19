SET(SRCS
    application.cpp
    application.h
    FileBrowser.cpp
    FileBrowser.h
    input.cpp
    input.h
    Renderer.cpp
    Renderer.h
)

target_sources(mnes PRIVATE ${SRCS})
