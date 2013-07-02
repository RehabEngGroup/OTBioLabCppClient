add_executable(OTBioLabFakedServer fakedServer.cpp)

target_link_libraries(OTBioLabFakedServer
      ${Boost_LIBRARIES} 
)

add_executable(OTBioLabClient OTBioLabClient.cpp connect.cpp)

target_link_libraries(OTBioLabClient 
      ${Boost_LIBRARIES} 
)

