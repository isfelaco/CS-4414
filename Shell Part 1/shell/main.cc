#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <fcntl.h>
#include <fstream>
#include <cstring>
#include <sys/wait.h> // Added for waitpid

using namespace std;

void parse_and_run_command(const std::string &command) {
    /*
    REQUIREMENTS:
        run simple commands (e.g. /bin/cat foo.txt bar.txt)
        input redirection (e.g. /usr/bin/gcc -E - < somefile.text)
        output redirection (e.g. /usr/bin/gcc -E - > somefile.text)
        pipelines of multiple commands (e.g. /bin/cat foo.txt | /bin/grep bar | /bin/grep baz > output.txt)
        builtin command exit
        outputs the exit status of each command
        prints out error messages to stderr (e.g. via std:cerr)
        does not involve the system's shell
    */
   
    // Parse command line
    // source: https://www.geeksforgeeks.org/split-a-sentence-into-words-in-cpp/
    vector<string> tokens;
    string tempWord;
    for (char c : command) {
        // Check if the character is a whitespace character
        if (isspace(c)) {
            // If the temporary string is not empty, add it to the list of words
            if (!tempWord.empty()) {
                tokens.push_back(tempWord);
                tempWord.clear();
            }
        } else {
            // If the character is not a whitespace character, add it to the temporary string
            tempWord.push_back(c);
        }
    }
    if (!tempWord.empty()) {
        tokens.push_back(tempWord);
    }

    bool redirect_input = false;
    string input_file;
    bool redirect_output = false;
    string output_file;
    
    string thisCommand;
    vector<string> args;

    // Find any exits or redirects first
    for (unsigned int i = 0; i < tokens.size(); ++i) {
        if (tokens[i] == "exit") {
            exit(0);
        }
        if (tokens[i][0] == '/') {
            thisCommand = tokens[i];
        }
        else if (tokens[i] == "<") {
            // input redirection
            if (i + 1 < tokens.size()) {
                // input redirection
                redirect_input = true;
                input_file = tokens[i + 1];
                i++; // Skip the next token (input file)
            } else {
                cerr << "Missing input file after '<'" << endl;
                return;
            }
        }
        else if (tokens[i] == ">") {
            // output redirection
            if (i + 1 < tokens.size()) {
                // output redirection
                redirect_output = true;
                output_file = tokens[i + 1];
                i++; // Skip the next token (output file)
            } else {
                cerr << "Missing output file after '>'" << endl;
                return;
            }
        }
        else {
            args.push_back(tokens[i]);
        }
        /* TODO: parse arguments and possibly save in an array */
    }

    // Run command

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
    // redirect first
        if (redirect_input) {
            /* TODO: not sure if this works yet, haven't tried a command that uses it */
            int fd =  open(output_file.c_str(), O_RDONLY);
            if (fd == -1) {
                perror("open");
                exit(1);
            }
            dup2(fd, STDIN_FILENO); // close stdin, copy file descriptor fd into standard input
            close(fd); // close file descriptor       
        }
        if (redirect_output) {
            int fd = open(output_file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd == -1) {
                perror("open");
                exit(1);
            }
            dup2(fd, STDOUT_FILENO); // close stdout, copy file descriptor fd into standard output
            close(fd); // close file descriptor
        }
         // Execute the command
        char *cmd = const_cast<char *>(thisCommand.c_str());
        char **argv = new char *[args.size() + 2]; // +2 for command and nullptr
        argv[0] = cmd;
        for (size_t i = 0; i < args.size(); ++i) {
            argv[i + 1] = const_cast<char *>(args[i].c_str());
        }
        argv[args.size() + 1] = nullptr;

        execvp(cmd, argv);
        // If execvp fails, handle the error and exit
        perror("execvp");
        exit(1);
    } else {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            // Print the exit status of the child process
            cout << "Child process exited with status: " << WEXITSTATUS(status) << endl;
        } else {
            cerr << "Child process did not exit normally." << endl;
        }
    }
    /* TODO: print exit status */

}

int main(void) {
    string command;
    cout << "> ";
    while (getline(cin, command)) {
        parse_and_run_command(command);
        cout << "> ";
    }
    return 0;
}
