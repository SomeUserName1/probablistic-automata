#include <catch2/catch.hpp>
#include <chrono>
#include <thread>
#include <sstream>
#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <memory>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>

#define PIPE_READ 0
#define PIPE_WRITE 1

std::string execute(const char*, const char*, std::string);

SCENARIO("The program can be called from the command line", "[CLI]") {
    GIVEN("No arguments") {
        WHEN("The program is executed") {
            std::filesystem::path p("test.txt");
            if(std::filesystem::exists(p)) {
                std::remove(p.c_str());
            }
            std::string output = execute("./ssm", "./ssm", "0");

            THEN("The text user interface is launched") {
                REQUIRE(output.starts_with("Hello"));
            }
        }
    }GIVEN("Incomplete arguments") {
        WHEN("the program is executed ") {
           /* std::string command("./ssm -t Reduce  -m WA > test.txt");
            int status = std::system(command.c_str());

            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            status = std::system("ps aux | grep ssm | awk '{print $2}' | xargs -I{} kill -9 {}");

            std::stringstream buf;
            buf << std::ifstream("test.txt").rdbuf();
*/
            THEN("The text user interface is launched") {
                // REQUIRE(buf.str().starts_with("Hello"));
            }
        }
    }GIVEN("Text and gui switches") {
        WHEN("The program is called with the gui switch") {
            THEN("An exception is thrown as its not implemented") {

            }
        }WHEN("The program is called with the TUI switch") {
            THEN("The TUI is executed") {

            }
        }
    }GIVEN("All arguments") {
        WHEN("The arguments are correctly specified") {

            THEN("The program executes correctly") {

            }
        }WHEN("An incorrect task is specified") {
            THEN("An exception is thrown") {

            }
        }WHEN("An incorrect model type is specified") {
            THEN("An exception is thrown") {

            }
        }WHEN("A non-existing input path is specified") {
            THEN("An exception is thrown") {

            }
        }WHEN("A badly formatted input path is specified") {
            THEN("An exception is thrown") {

            }
        }WHEN("A badly formatted output path is specified") {
            THEN("An exception is thrown") {

            }
        }
    }
}

std::string execute(const char* cmd, const char* args, std::string msg) {
    int aStdinPipe[2];
    int aStdoutPipe[2];
    char output[1024];
    int nResult;
    std::string result;

    if (pipe(aStdinPipe) < 0) {
        throw std::runtime_error("allocating pipe for child input redirect");
    }
    if (pipe(aStdoutPipe) < 0) {
        close(aStdinPipe[PIPE_READ]);
        close(aStdinPipe[PIPE_WRITE]);
        throw std::runtime_error("allocating pipe for child output redirect");
    }

    pid_t pid = fork();

    if (pid == 0)
    {
        // redirect stdin
        if (dup2(aStdinPipe[PIPE_READ], STDIN_FILENO) == -1) {
            throw std::runtime_error("duplicating stdin failed");
        }

        // redirect stdout
        if (dup2(aStdoutPipe[PIPE_WRITE], STDOUT_FILENO) == -1) {
            throw std::runtime_error("duplicating stdout failed");
        }

        // redirect stderr
        if (dup2(aStdoutPipe[PIPE_WRITE], STDERR_FILENO) == -1) {
            throw std::runtime_error("duplicating stderr failed");
        }

        // all these are for use by parent only
        close(aStdinPipe[PIPE_READ]);
        close(aStdinPipe[PIPE_WRITE]);
        close(aStdoutPipe[PIPE_READ]);
        close(aStdoutPipe[PIPE_WRITE]);

        // run child process image
        // replace this with any exec* function find easier to use ("man exec")
        nResult = execlp(cmd, args, (char *) NULL);
        exit(nResult);
        //std::system(cmd);
    }
    else if (pid > 0) {
        // close unused file descriptors, these are for child only
        close(aStdinPipe[PIPE_READ]);
        close(aStdoutPipe[PIPE_WRITE]);

        write(aStdinPipe[PIPE_WRITE], msg.c_str(), msg.size());

        // Just a char by char read here, you can change it accordingly
        while (read(aStdoutPipe[PIPE_READ], &output, 1024) == 1) {
            result.append(output);
        }

        // done with these in this example program, you would normally keep these
        // open of course as long as you want to talk to the child
        close(aStdinPipe[PIPE_WRITE]);
        close(aStdoutPipe[PIPE_READ]);
    } else {
        // failed to create child
        close(aStdinPipe[PIPE_READ]);
        close(aStdinPipe[PIPE_WRITE]);
        close(aStdoutPipe[PIPE_READ]);
        close(aStdoutPipe[PIPE_WRITE]);
        throw std::runtime_error("Fork failed");
    }
    return output;
}

/*
 * int aStdinPipe[2];
    if (pipe(aStdinPipe) < 0) {
        throw std::runtime_error("allocating pipe for child input redirect failed");
    }

    pid_t pid = fork();

    if (pid == 0)
    {
        if (dup2(aStdinPipe[PIPE_READ], STDIN_FILENO) == -1) {
            throw std::runtime_error("child input redirect failed");
        }
        close(aStdinPipe[PIPE_READ]);
        close(aStdinPipe[PIPE_WRITE]);


        std::system(cmd);
    }
    else if (pid > 0)
    {   std::string msgExit("0");
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        close(aStdinPipe[PIPE_READ]);
        write(aStdinPipe[PIPE_WRITE], msgExit.c_str(), msgExit.size());
        close(aStdinPipe[PIPE_WRITE]);
    }
    else
    {
        close(aStdinPipe[PIPE_READ]);
        close(aStdinPipe[PIPE_WRITE]);
        throw std::runtime_error("Fork failed");
    }
 */