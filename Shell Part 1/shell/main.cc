#include <cstdlib>
#include <iostream>
#include <string>

void parse_and_run_command(const std::string &command) {
    /* TODO: Implement this. */
    /* Note that this is not the correct way to test for the exit command.
       For example the command "   exit  " should also exit your shell.
     */
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
    if (command == "exit") {
        exit(0);
    }
    std::cerr << "Not implemented.\n";
}

int main(void) {
    std::string command;
    std::cout << "> ";
    while (std::getline(std::cin, command)) {
        parse_and_run_command(command);
        std::cout << "> ";
    }
    return 0;
}
