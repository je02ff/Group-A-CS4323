cmake_minimum_required(VERSION 3.20)
project(Group_A C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")


include_directories(.)

add_executable(Group_A
        JacobWilson.h
        JacobWilson.c
        readDatabaseIntoArray.h
        Roden.c
        Roden.h
        tcpStuff.h
        dataServer.c)

add_executable(jermiah
        misc/clientCOPY.c
        misc/clientCOPY.h)

add_executable(modeL_All_Fix
        Demos/5_All_Fixed/dsModel_All_Fix.c
        )
add_executable(JacobDeadlock
        Demos/3_DeadLock_Scenarios/DeadlockDemo.c
        )

add_executable(JacobRace
        Demos/1_Race_Cond_Examples/RaceConditionDemo.c
        )

add_executable(writeTest
        ./misc/writeTest.c
        JacobWilson.c
        JacobWilson.h
        readDatabaseIntoArray.h
        Roden.c
        )
add_executable(dbTEST
        JacobWilson.h
        JacobWilson.c
        readDatabaseIntoArray.h
        Roden.c
        Roden.h
        tcpStuff.h
        dataServerTEST.c)