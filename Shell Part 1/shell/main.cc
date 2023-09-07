#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <fcntl.h>
#include <fstream>
#include <cstring>
#include <sys/wait.h>

using namespace std;

void parse_and_run_command(const string &command) {
    /*
    REQUIREMENTS:
        run simple commands (e.g. /bin/cat foo.txt bar.txt)
        input redirection (e.g. /usr/bin/gcc -E < somefile.txt)
        output redirection (e.g. /usr/bin/gcc -E > somefile.txt)
        pipelines of multiple commands (e.g. /bin/cat foo.txt | /bin/grep bar | /bin/grep baz > output.txt)
        builtin command exit
        outputs the exit status of each command
        prints out error messages to stderr (e.g. via std:cerr)
        does not involve the system's shell
    */
   
    /* PARSE THE INPUT */
    /* Parse the commnd line, separating tokens by whitespace */
    /* source: https://www.geeksforgeeks.org/split-a-sentence-into-words-in-cpp/ */
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
    /* end source */

    bool redirect_input = false;
    bool redirect_output = false;
    const char* input_file;
    const char* output_file;
    
    string cmd_str;
    vector<string> cmd_args;

    /* Find any exits or redirects in the tokens */
    for (unsigned int i = 0; i < tokens.size(); ++i) {
        string token = tokens[i];
        if (token == "exit") {
            exit(0);
        }
        else if (token == "<") {
            // next token is the input file
            if (i + 1 < tokens.size()) {
                redirect_input = true;
                if (tokens[i+1] == "<" || tokens[i+1] == ">") {
                    cerr << "invalid command" << endl;
                }
                input_file = tokens[i + 1].c_str();
                i++;
            } else {
                cerr << "invalid command" << endl;
                return;
            }
        }
        else if (token == ">") {
            // next token is the output file
            if (i + 1 < tokens.size()) {
                redirect_output = true;
                if (tokens[i+1] == "<" || tokens[i+1] == ">") {
                    cerr << "invalid command" << endl;
                }
                output_file = tokens[i + 1].c_str();
                i++;
            } else {
                cerr << "invalid command" << endl;
                return;
            }
        }
           else if (cmd_str.empty()) {
            // token is the command
            cmd_str = token;
        }
        else {
            // token is an argument to the command
            cmd_args.push_back(token);
        }
    }

    /* EXECUTE THE COMMAND */
    /* Fork */
    pid_t pid = fork();

    if (pid == -1) { // fork error
        cerr << "Fork failed" << endl;
        cout << "> " << endl;
        exit(1);
    } else if (pid == 0) { // child proces
        /* Redirect first */
        if (redirect_input) {
            int fd =  open(input_file, O_RDONLY);
            if (fd == -1) {
                perror("open"); // need to fix i think
                exit(1);
            }
            dup2(fd, STDIN_FILENO); // close stdin, copy file descriptor fd into standard input
            close(fd); // close file descriptor       
        }
        if (redirect_output) {
            int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd == -1) {
                perror("open"); // need to fix i think
                exit(1);
            }
            dup2(fd, STDOUT_FILENO); // close stdout, copy file descriptor fd into standard output
            close(fd); // close file descriptor
        }

        /* Convert string vector to char array to pass to execvp */
        char *cmd = const_cast<char *>(cmd_str.c_str());
        char **argv = new char *[cmd_args.size() + 2]; // +2 for command and nullptr
        argv[0] = cmd;
        for (size_t i = 0; i < cmd_args.size(); ++i) {
            argv[i + 1] = const_cast<char *>(cmd_args[i].c_str());
        }
        argv[cmd_args.size() + 1] = nullptr;


        /* Run the command */
        int exec_status = execv(cmd, argv);
        if (exec_status == -1) {
            cerr << "invalid command / Command not found" << endl;
            exit(1);
        }
    } else { // parent process
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            /* Print the exit status of the child process */
            cout << cmd_str << " exit status: " << WEXITSTATUS(status) << endl;
        }
    }
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
