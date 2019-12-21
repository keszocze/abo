add_executable(minimize_adders minimize_adders.cpp)

target_link_libraries(minimize_adders
	PRIVATE bucket_minimization
)

add_executable(minimize_iscas minimize_iscas.cpp)

target_link_libraries(minimize_iscas
        PRIVATE bucket_minimization
)
