#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <fcntl.h>
#include <fstream>
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

    // Find any exits or redirects first
    for (unsigned int i = 0; i < tokens.size(); ++i) {
        if (tokens[i] == "exit") {
            exit(0);
        }
        if (tokens[i] == "<") {
            // input redirection
            redirect_input = true;
            input_file = tokens[i+1];
        }
        if (tokens[i] == ">") {
            // output redirection
            redirect_output = true;
            output_file = tokens[i+1];
        }
    }

    // Run command
    for (string token : tokens) {
        if (token[0] == '/') {
            // redirect first
            if (redirect_input) {
                cout << "Redirect Input" << endl;         
            }
            if (redirect_output) {
                int fd;
                if ((fd = open(output_file.c_str(), O_WRONLY)) == 1) {
                    perror("open");
                }
                dup2(fd, STDOUT_FILENO); // close stdout, copy file descriptor fd into standard output
                close(fd); // close file descriptor
            }
            string com = "ls";
            execlp(token.c_str(), com.c_str(), NULL);
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
