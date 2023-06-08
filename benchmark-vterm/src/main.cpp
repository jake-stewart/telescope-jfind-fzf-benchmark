#include "../include/dterm.hpp"
#include "../include/ansi_wrapper.hpp"
#include "../include/dterm_screen_printer.hpp"
#include "../include/util.hpp"
#include "../include/option_parser.hpp"
#include <locale.h>
#include <thread>
#include <unistd.h>
#include <chrono>
#include <vterm.h>

struct Options {
    std::string setup = "";
    int setupDelay = 50;
    int postSetupDelay = 1000;

    std::string input = "";
    int inputDelay = 0;

    std::string output = "";
    int outputLine = 0;
};

#define MILLISECONDS 1000
DTerm term({24, 80});
AnsiWrapper &ansi = AnsiWrapper::instance();
std::chrono::time_point<std::chrono::system_clock> start, end;
Options options;

bool parseOptions(int argc, const char **argv, Options *options) {
    std::string historyFile;

    std::vector<Option*> parseOptions = {{
        new StringOption("setup", &options->setup),
        (new IntegerOption("setup-key-delay", &options->setupDelay))->min(0),
        (new IntegerOption("post-setup-delay", &options->postSetupDelay))->min(0),
        new StringOption("input", &options->input),
        new StringOption("output", &options->output),
        (new IntegerOption("output-line", &options->outputLine))->min(1),
        (new IntegerOption("input-key-delay", &options->inputDelay))->min(0),
    }};

    OptionParser optionParser(parseOptions);
    return optionParser.parse(argc, argv);
}

void inputThread() {
    for (char c : options.setup) {
        term.getProcess()->input(c);
        usleep(options.setupDelay * MILLISECONDS);
    }
    usleep(options.postSetupDelay * MILLISECONDS);
    start = std::chrono::system_clock::now();
    for (char c : options.input) {
        term.getProcess()->input(c);
        usleep(options.inputDelay * MILLISECONDS);
    }
}

int main(int argc, const char **argv) {
    vterm_check_version(VTERM_VERSION_MAJOR, VTERM_VERSION_MINOR);
    setlocale(LC_CTYPE, nullptr);

    if (!parseOptions(argc, argv, &options)) {
        return 1;
    }

    term.getScreen()->clear();
    term.startProcess("/bin/bash");

    ansi.initTerm();
    DTermScreenPrinter printer;
    printer.setScreen(term.getScreen());

    std::thread t(&inputThread);
    t.detach();

    bool found = false;
    int i = 1000;
    while (term.getProcess()->getState() == ProcessState::Active) {
        i++;
        if (term.getScreen()->isDirty()) {
            if (i > 100) {
                i = 0;
                ansi.moveHome();
                printer.printBordered();
                fflush(stdout);
            }

            if (term.getScreen()->contains(options.outputLine - 1, options.output)) {
                end = std::chrono::system_clock::now();
                found = true;
                break;
            }

        }
        usleep(1 * MILLISECONDS);
    }


    ansi.restoreTerm();
    term.endProcess();

    if (found) {
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        printf("%lld\n", milliseconds.count());
    }

    return 0;
}
