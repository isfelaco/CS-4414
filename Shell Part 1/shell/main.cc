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
    /* PARSE THE INPUT */
    /* Parse the commnd line, separating tokens by whitespace */
    /* source: https://www.geeksforgeeks.org/split-a-sentence-into-words-in-cpp/ */
    vector<vector<string>> commands;
    vector<string> tokens;;
    string tempWord;
    for (unsigned int i = 0; i < command.length(); ++i) {
        char c = command[i];
        // Check if the character is a whitespace character
        if (isspace(c)) {
            // piping character must be surrounded by spaces
            if (command[i+1] == '|' && isspace(command[i+2])) {                
                if (!tempWord.empty()) {
                    tokens.push_back(tempWord);
                    tempWord.clear();
                }
                commands.push_back(tokens);
                tokens.clear();
                i += 2;
            }
            // there is no command after the piping character
            else if (command[i+1] == '|' && i+2 == command.length()) {
                cerr << "invalid command" << endl;
                exit(1);
            }
            else if (!tempWord.empty()) {
                tokens.push_back(tempWord);
                tempWord.clear();
            }
            
        }
        else tempWord.push_back(c);
    }
    if (!tempWord.empty()) tokens.push_back(tempWord);
    if (!tokens.empty()) commands.push_back(tokens);
    /* end source */

    int prev_pipefd[2];
    vector<int> pidlist;

    for (unsigned int j = 0; j < commands.size(); j++) {
        vector<string> c = commands[j];
        bool redirect_input = false;
        bool redirect_output = false;
        const char* input_file;
        const char* output_file;
        
        string cmd_str;
        vector<string> cmd_args;
        /* Find any exits or redirects in the tokens */
        for (unsigned int i = 0; i < c.size(); ++i) {
            string token = c[i];
            if (token == "exit") {
                exit(0);
            }
            else if (token == "<") {
                // next token is the input file
                if (i + 1 < c.size()) {
                    redirect_input = true;
                    if (c[i+1] == "<" || c[i+1] == ">") cerr << "invalid command" << endl;
                    input_file = c[i + 1].c_str();
                    i++;
                } else {
                    cerr << "invalid command" << endl;
                    return;
                }
            }
            else if (token == ">") {
                // next token is the output file
                if (i + 1 < c.size()) {
                    redirect_output = true;
                    if (c[i+1] == "<" || c[i+1] == ">") cerr << "invalid command" << endl;
                    output_file = c[i + 1].c_str();
                    i++;
                } else {
                    cerr << "invalid command" << endl;
                    return;
                }
            }
            // first token that is not a redirection token is the command
            else if (cmd_str.empty()) cmd_str = token;
            // token is an argument to the command
            else cmd_args.push_back(token);
        }

        /* EXECUTE THE COMMAND */
        /* Pipe */
        int pipefd[2];
        if (j >= 0 && j < commands.size() - 1) { // not the last command
            if (pipe(pipefd)) {
                perror("pipe");
                exit(1);
            }
        }

		
        /* Fork */
        pid_t pid = fork();
        pidlist.push_back(pid); // add pid to list of pids that need to be waited

        if (pid == -1) cerr << "Fork failed" << endl; // fork error
        else if (pid == 0) { // child process
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

            if (j > 0) { // not the first command
                dup2(prev_pipefd[0], STDIN_FILENO); // read from the write end of previous pipe
                close(prev_pipefd[0]); // close original read end of the pipe
                close(prev_pipefd[1]); // close write end of pipe in the current process
            }
            if (j < commands.size() - 1) { // not last command   
                close(pipefd[0]); // close read end of pipe in current process
                dup2(pipefd[1], STDOUT_FILENO); // open the pipe write end to write
                close(pipefd[1]);
            }

            /* Run the command */
            if (access(cmd, X_OK) == 0) { // Check if the command is executable
                int exec_status = execv(cmd, argv);
                if (exec_status == -1) {
                    cerr << "invalid command / Command not found" << endl;
                    exit(1);
                }
            } else {
                cerr << "invalid command / Command not found" << endl;
                exit(1);
            }

            delete[] argv;
        } else { // parent process
            if (j > 0) {
                close(prev_pipefd[0]);
                close(prev_pipefd[1]);
            }

            prev_pipefd[0] = pipefd[0];
            prev_pipefd[1] = pipefd[1];
            
            if (j == commands.size() - 1) { // start waiting if j is final command            	
            	for (unsigned int ppid = 0; ppid < pidlist.size(); ppid++) {
                    int status;
            		waitpid(pidlist[ppid], &status, 0); // loop to wait pids
                    if (WIFEXITED(status)) {
                        /* Print the exit status of the child process */
                        cout << commands[ppid][0] << " exit status: " << WEXITSTATUS(status) << endl;
                    }
            	} 
            }
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
