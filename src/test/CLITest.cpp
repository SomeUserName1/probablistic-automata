#include <catch2/catch.hpp>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

#include <csignal>
#include <cstdio>
#include <cstring>
#include <eigen3/Eigen/Eigen>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>

#include "../ui/UserInterface.h"
#include "TestUtils.h"

constexpr uint PIPE_READ = 0;
constexpr uint PIPE_WRITE = 1;

static auto execute(const std::string &cmd, std::vector<std::string> args,
                    const std::string &msg) -> std::string;

SCENARIO("The program can be called from the command line", "[CLI]") {
  GIVEN("No arguments") {
    WHEN("The program is executed") {
      std::string output = execute("./ssm", {"/.ssm"}, "4\n");
      THEN("The text user interface is launched") {
        REQUIRE(output.starts_with("Hello"));
      }
    }
  }
  GIVEN("Incomplete arguments") {
    WHEN("the program is executed ") {
      std::string output =
          execute("./ssm", {"./ssm", "-t", "Reduction", "-m", "WA"}, "4\n");
      THEN("The text user interface is launched") {
        REQUIRE(output.starts_with("Hello"));
      }
    }
  }
  GIVEN("Text or gui switches") {
    WHEN("The program is called with the gui switch") {
      std::string output = execute("./ssm", {"./ssm", "-G"}, "");
      THEN("An error message is displayed as its not implemented") {
        REQUIRE(output.ends_with("NotImplemented\n"));
      }
    }
    WHEN("The program is called with the TUI switch") {
      std::string output = execute("./ssm", {"./ssm", "-T"}, "4\n");
      THEN("The TUI is executed") { REQUIRE(output.starts_with("Hello")); }
    }
  }
  GIVEN("All arguments") {
    WHEN("The arguments are correctly specified") {
      std::vector<std::string> args = {
          "./ssm",  "-t",     "Reduction",
          "-m",     "WA",     "-r",
          "Kiefer", "-i",     "../src/test/test_input_sparse.txt",
          "-o",     "out.txt"};
      std::string output = execute("./ssm", args, "");
      THEN("The program executes without error, loading the input, processing "
           "and writing the output to file") {
        std::string line;
        std::size_t pos = 0, prev = 0;
        pos = output.find('\n', 0);
        line = output.substr(prev, pos - prev);
        prev = pos + 1;
        REQUIRE(line.starts_with("Loading"));

        pos = output.find('\n', prev);
        line = output.substr(prev, pos - prev);
        prev = pos + 1;

        REQUIRE(line.starts_with("Parsed"));

        pos = output.find('\n', prev);
        line = output.substr(prev, pos - prev);
        prev = pos + 1;

        REQUIRE(line.starts_with("Finished"));

        pos = output.find('\n', prev);
        line = output.substr(prev, pos - prev);
        prev = pos + 1;

        REQUIRE(line.starts_with("Output"));
      }
    }
    WHEN("An incorrect task is specified") {
      std::vector<std::string> args = {
          "./ssm",  "-t",     "Reduc",
          "-m",     "WA",     "-r",
          "Kiefer", "-i",     "../src/test/test_input_sparse.txt",
          "-o",     "out.txt"};
      std::string output = execute("./ssm", args, "");
      THEN("An error message is displayed") {
        REQUIRE(output.ends_with("you  specified Reduc\n"));
      }
    }
    WHEN("An incorrect model type is specified") {
      std::vector<std::string> args = {"./ssm",
                                       "-t Reduction",
                                       "-m VfB",
                                       "-r Kiefer",
                                       "-i ../src/test/test_input_sparse.txt",
                                       "-o out.txt"};
      std::string output = execute("./ssm", args, "");
      THEN("An error message is displayed") {
        REQUIRE(output.ends_with("as model, you specified VfB\n"));
      }
    }
    WHEN("A non-existing input path is specified") {
      std::vector<std::string> args = {"./ssm",        "-t", "Reduction", "-m",
                                       "WA",           "-r", "Kiefer",    "-i",
                                       "/sparseDense", "-o", "out.txt"};
      std::string output = execute("./ssm", args, "");
      THEN("An exception is thrown") {
        REQUIRE(output.ends_with("No such file or directory [/sparseDense]\n"));
      }
    }
    WHEN("A badly formatted input path is specified") {
      std::vector<std::string> args = {"./ssm",   "-t", "Reduction", "-m",
                                       "WA",      "-r", "Kiefer",    "-i",
                                       "-+!?**/", "-o", "out.txt"};
      std::string output = execute("./ssm", args, "");
      THEN("An exception is thrown") {
        REQUIRE(output.ends_with("No such file or directory [-+!?**/]\n"));
      }
    }
    WHEN("A badly formatted output path is specified") {
      std::vector<std::string> args = {
          "./ssm",  "-t",    "Reduction",
          "-m",     "WA",    "-r",
          "Kiefer", "-i",    "../src/test/test_input_sparse.txt",
          "-o",     "/home/"};
      std::string output = execute("./ssm", args, "");
      THEN("An exception is thrown") {
        REQUIRE(output.ends_with("Please specify a path with a file name to "
                                 "write the results to!\n"));
      }
    }
  }
}

SCENARIO("Equivalence") {
  GIVEN("The running example and a program reduced version") {
    std::vector<std::string> args = {"./ssm",
                                     "-t",
                                     "Reduction",
                                     "-m",
                                     "WA",
                                     "-r",
                                     "Kiefer",
                                     "-i",
                                     "../src/test/test_input_dense.txt",
                                     "-o",
                                     "out_eq_test.txt"};
    execute("./ssm", args, "");
    WHEN("Using the output of the program ") {
      std::string progOutput = UserInterface::read_file("out_eq_test.txt");
      std::string minimized = progOutput.substr(
          progOutput.find("After Reduction\n") + 16, progOutput.size());
      UserInterface::display_file(minimized, "result.txt");
      THEN("It's equivalent to the original automaton") {
        args = {"./ssm",
                "-t",
                "Equivalence",
                "-m",
                "WA",
                "-i",
                "../src/test/test_input_dense.txt",
                "-c",
                "result.txt",
                "-o",
                "out_result.txt"};
        minimized = execute("./ssm", args, "");
        progOutput = UserInterface::read_file("out_result.txt");
        std::string check("equivalent");
        std::cout << minimized << std::endl;
        REQUIRE(std::equal(progOutput.begin(), progOutput.end(), check.begin(),
                           [](auto a, auto b) { return a == b; }));
      }
    }
  }
}

static auto execute(const std::string &cmd, std::vector<std::string> args,
                    const std::string &msg) -> std::string {
  int stdinPipe[2];
  int stdoutPipe[2];
  char output = 0;
  int nResult = 0;
  std::string result;

  if (pipe(stdinPipe) < 0) {
    throw std::runtime_error("allocating pipe for child input redirect");
  }
  if (pipe(stdoutPipe) < 0) {
    close(stdinPipe[PIPE_READ]);
    close(stdinPipe[PIPE_WRITE]);
    throw std::runtime_error("allocating pipe for child output redirect");
  }

  pid_t pid = fork();
  if (pid == 0) {
    if (dup2(stdinPipe[PIPE_READ], STDIN_FILENO) == -1 ||
        dup2(stdoutPipe[PIPE_WRITE], STDOUT_FILENO) == -1 ||
        dup2(stdoutPipe[PIPE_WRITE], STDERR_FILENO) == -1) {
      throw std::runtime_error("duplicating stdin, stdout or stderr failed");
    }
    close(stdinPipe[PIPE_READ]);
    close(stdinPipe[PIPE_WRITE]);
    close(stdoutPipe[PIPE_READ]);
    close(stdoutPipe[PIPE_WRITE]);

    std::vector<char *> argv;
    std::transform(std::begin(args), std::end(args), std::back_inserter(argv),
                   [](std::string &s) {
                     s.push_back(0);
                     return &s[0];
                   });
    argv.push_back(nullptr);

    nResult = execvp(cmd.c_str(), argv.data());
    exit(nResult);
  } else if (pid > 0) {
    close(stdinPipe[PIPE_READ]);
    close(stdoutPipe[PIPE_WRITE]);

    write(stdinPipe[PIPE_WRITE], msg.c_str(), msg.size());

    while (read(stdoutPipe[PIPE_READ], &output, 1) == 1) {
      result += output;
    }

    close(stdinPipe[PIPE_WRITE]);
    close(stdoutPipe[PIPE_READ]);
  } else {
    close(stdinPipe[PIPE_READ]);
    close(stdinPipe[PIPE_WRITE]);
    close(stdoutPipe[PIPE_READ]);
    close(stdoutPipe[PIPE_WRITE]);
    throw std::runtime_error("Fork failed");
  }
  return result;
}
