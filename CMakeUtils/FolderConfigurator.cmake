cmake_minimum_required(VERSION 3.25)

set(Engine_Dir ${CMAKE_CURRENT_BINARY_DIR})
set(Engine_Content_Dir ${Engine_Dir}/Content)

#add_definitions(-DENGINE_DIR ${Engine_Dir})
#add_definitions(-DENGINE_CONTENT_DIR ${Engine_Content_Dir})

function(ConfigureProjectFolders)
    file(MAKE_DIRECTORY ${Engine_Content_Dir})
endfunction()

function(CopyResourcesToEngine)
#    file(COPY DESTINATION  )
endfunction()