add_executable(dumprwtree
    dumprwtree.cpp
)

target_link_libraries(dumprwtree
    PUBLIC
        librw
)

if(LIBRW_INSTALL)
    install(TARGETS dumprwtree
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    )
endif()
