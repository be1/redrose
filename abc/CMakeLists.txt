project(abc)

# Crée des variables avec les fichiers à compiler
set(SRCS
    abc.c
    parser.c
    )
    
set(HEADERS
    abc.h
    parser.h
    )

add_library(abc ${SRCS} ${HEADERS})

target_include_directories(abc INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
