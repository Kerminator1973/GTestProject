cmake_minimum_required(VERSION 3.14)
project(gRPCServerExample LANGUAGES CXX)

# Find gRPC and Protobuf packages
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

# Proto files
set(PROTO_DIR "${CMAKE_CURRENT_SOURCE_DIR}/proto")
set(GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")

file(MAKE_DIRECTORY ${GENERATED_DIR})

# Generate C++ code from .proto
#protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_DIR}/helloworld.proto)
#grpc_generate_cpp(GRPC_SRCS GRPC_HDRS ${PROTO_DIR}/helloworld.proto)

# Move generated files to a known location
#set(GENERATED_SRCS ${GENERATED_DIR}/helloworld.grpc.pb.cc ${GENERATED_DIR}/helloworld.pb.cc)
#set(GENERATED_HDRS ${GENERATED_DIR}/helloworld.grpc.pb.h ${GENERATED_DIR}/helloworld.pb.h)

# Server target
add_executable(server 
	server.cpp
	helloworld.grpc.pb.cc 
	helloworld.pb.cc
	helloworld.grpc.pb.h 
	helloworld.pb.h
)
target_include_directories(server PRIVATE ${GENERATED_DIR})
target_link_libraries(server PRIVATE gRPC::grpc++ protobuf::libprotobuf)

# Optional: set C++17
target_compile_features(server PRIVATE cxx_std_17)

