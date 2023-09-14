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
    vector<vector<string>> commands;
    vector<string> tokens;;
    string tempWord;
    for (char c : command) {
        // Check if the character is a whitespace character
        if (isspace(c)) {
            // If the temporary string is not empty, add it to the list of words
            if (!tempWord.empty()) {
                tokens.push_back(tempWord);
                tempWord.clear();
            }
        }
        else if (c == '|') {
            if (!tempWord.empty()) {
                tokens.push_back(tempWord);
            }
            commands.push_back(tokens);
            tokens.clear();
        } else {
            // If the character is not a whitespace character, add it to the temporary string
            tempWord.push_back(c);
        }
    }
    if (!tempWord.empty()) {
        tokens.push_back(tempWord);
    }
    if (!tokens.empty()) {
        commands.push_back(tokens);
    }
    /* end source */

    bool redirect_input = false;
    bool redirect_output = false;
    const char* input_file;
    const char* output_file;
    
    string cmd_str;
    vector<string> cmd_args;

    for (vector<string> c : commands) {
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
                    if (c[i+1] == "<" || c[i+1] == ">" || c[i+1] == "|") {
                        cerr << "invalid command" << endl;
                    }
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
                    if (c[i+1] == "<" || c[i+1] == ">" || c[i+1] == "|") {
                        cerr << "invalid command" << endl;
                    }
                    output_file = c[i + 1].c_str();
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

        /*
        if (i >= 0 && i < totalcmd-1) // there are multiple commands
            pipe(int pipefd[2]) // pipe array is used to return two file descriptors referring to the ends of the pipe
        Pid = Fork()
        if (pid==0) // in child process
            Do redirection if there is any redirection in this command
        if (i > 0) // if command not the first command
            close(stdin)
            dup(previous_pipe_out) // read from the write end of previous pipe
        if (i < totalcmds - 1) // not last command ?
            close(stdout) // close out file
            dup(pipe_out) // open the pipe write end to write
        close the pipes that are not needed

        if it is the last command, we want to write to the termina/redirection file
        */
        
        // TESTING
        // if there are multiple commands?
        int pipefd[2]; // init the pipefds?
        if (c != commands.back()) { // if not final command
        	cout << "HELLO" << endl; // test message
        	
        	
        	if (pipe(pipefd) < 0) { // create pipe check for errors
        		cerr << "Pipe failed" << endl; // if pipe fails
        		cout << "> " << endl;
        	} 
        }
		
		// need to do another fork???
		// where does the second fork go?
		
        /* Fork */
        pid_t pid = fork();
        // pid = fork();

        if (pid == -1) { // fork error
            cerr << "Fork failed" << endl;
            cout << "> " << endl;
            exit(1);
        } else if (pid == 0) { // child process
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
            
            // PIPE TESTING HERE
            
            if (c != commands[0]) { // if not first command in pipeline
            	dup2(pipefd[0], STDIN_FILENO); // close stdin, use pipefd[0]?
            	// need to get your input from the previous command instead of stdin
            	// need to read from read end of previous pipe
            }
            else { // if first command in pipeline
            	close(pipefd[0]); // close pipe read end?
            }
            
            
            if (c != commands.back()) { // if not last command in pipeline
            	dup2(pipefd[1], STDOUT_FILENO); // close stdout, use pipefd[1]?
            	// need to give your output to the next command instead of stdout
            	// need to write to write end of pipe
            }
            else { // if last command in pipeline
            	close(pipefd[1]); // close pipe write end?
            }
            
           //  close(pipefd[0]);
           // TODO: understand when and where to close unused fd's? 
           // TODO: figure out where the second fork goes (one for producer one for consumer?)

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
            // close(pipefd[0]);
            // close(pipefd[1]);
        } else { // parent process
        	// need to close unneeded fds??
        	close(pipefd[0]); // do we need these?
        	close(pipefd[1]);
        	
            int status;
            waitpid(pid, &status, 0);
            if (WIFEXITED(status)) {
                /* Print the exit status of the child process */
                cout << cmd_str << " exit status: " << WEXITSTATUS(status) << endl;
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
