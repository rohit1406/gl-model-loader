//
//  Logger.h
//  Breakout
//
//  Created by Apple on 01/07/21.
//

#ifndef Logger_h
#define Logger_h

#include <iostream>
#include <fstream>      // For file operations

// for time calculations
#include <chrono>
#include <ctime>

static const std::string logFileName = "logger.txt";
static std::ofstream logFile;

static void LOGGER(std::string message){
    logFile.open(logFileName, std::ios::out | std::ios::app);
    
    if(logFile.is_open()){
        auto currentTime = std::chrono::system_clock::now();
        std::time_t startTime = std::chrono::system_clock::to_time_t(currentTime);
        std::string dateTime = std::string(std::ctime(&startTime));
        logFile << message << ": " << dateTime;
        logFile.close();
    }else{
        std::cout << "Unable to open a file " << logFileName << std::endl;
    }
}
#endif /* Logger_h */
