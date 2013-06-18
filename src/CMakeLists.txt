add_executable(OTBioLabClient OTBioLabClient.cpp connect.cpp)


target_link_libraries(OTBioLabClient 
      ${Boost_LIBRARIES} 
)

