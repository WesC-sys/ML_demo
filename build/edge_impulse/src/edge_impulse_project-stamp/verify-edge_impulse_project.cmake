# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

if("C:/nordic/myApps/ML_demo/src/model.zip" STREQUAL "")
  message(FATAL_ERROR "LOCAL can't be empty")
endif()

if(NOT EXISTS "C:/nordic/myApps/ML_demo/src/model.zip")
  message(FATAL_ERROR "File not found: C:/nordic/myApps/ML_demo/src/model.zip")
endif()

if("" STREQUAL "")
  message(WARNING "File will not be verified since no URL_HASH specified")
  return()
endif()

if("" STREQUAL "")
  message(FATAL_ERROR "EXPECT_VALUE can't be empty")
endif()

message(STATUS "verifying file...
     file='C:/nordic/myApps/ML_demo/src/model.zip'")

file("" "C:/nordic/myApps/ML_demo/src/model.zip" actual_value)

if(NOT "${actual_value}" STREQUAL "")
  message(FATAL_ERROR "error:  hash of
  C:/nordic/myApps/ML_demo/src/model.zip
does not match expected value
  expected: ''
    actual: '${actual_value}'
")
endif()

message(STATUS "verifying file... done")
