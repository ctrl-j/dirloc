#include <iostream>
#include <QApplication>
#include <QCoreApplication>
#include <string>

#include "dirloc.hh"
#include "MainWindow.hh"

fpath search_path;
FILESET fs;
bool skipped_lines_print;

enum class arg {
    cli_default,
    cli_graph,
    gui,
    version,
    help,
    unknown
    // TODO: options for other language files
    // TODO: options for ignoring specific file extensions
    // TODO: option to recursively search sub-directories
};

arg hashArgString(int argc, char *argv[]) {
    fs = CPP_FILES;
    search_path = std::filesystem::current_path();
    skipped_lines_print = false;
    if (argc == 1) return arg::cli_default;

    const std::string cmd = argv[1];
    if (argc == 2) {
        if (cmd == "-graph" || cmd == "-g") return arg::cli_graph;
        if (cmd == "--gui") return arg::gui;
        if (cmd == "-s" || cmd == "-skips") {
            skipped_lines_print = true;
            return arg::cli_default;
        }
        if (cmd == "--help" || cmd == "--h") return arg::help;
        if (cmd == "--version" || cmd == "--v") return arg::version;
        if (cmd == "-c" || cmd == "-c-only") {
            fs = C_FILES;
            return arg::cli_default;
        }
        else return arg::unknown;
    }

    else if (argc == 3) {
        if (cmd == "-e" ||
            cmd == "-ext" ||
            cmd == "-external") {
            search_path = argv[2];
            return arg::cli_default;
        } else if (cmd == "-c" || cmd == "-c-only") {
            fs = C_FILES;
            if (str(argv[2]) == "-graph" || str(argv[2]) == "-g") return arg::cli_graph;
            if (str(argv[2]) == "--gui") return arg::gui;
            if (cmd == "-s" || cmd == "-skips") {
                skipped_lines_print = true;
                return arg::cli_default;
            }
            if (str(argv[2]) == "--help" || str(argv[2]) == "--h") return arg::help;
            if (str(argv[2]) == "--version" || str(argv[2]) == "--v") return arg::version;
            else return arg::unknown;
        } else return arg::unknown;
    }

    else if (argc == 4) {
        str cmd1 = argv[2];
        str cmd2 = argv[3];
        if (cmd1 == "-e" ||
            cmd1 == "-ext" ||
            cmd1 == "-external") {
            search_path = argv[3];

            if (cmd == "-graph" || cmd == "-g") return arg::cli_graph;
            if (cmd == "--gui") return arg::gui;
            if (cmd == "-s" || cmd == "-skips") {
                skipped_lines_print = true;
                return arg::cli_default;
            }
            if (cmd == "-c" || cmd == "-c-only") {
                fs = C_FILES;
                return arg::cli_default;
            }
        }
        else {
            if (cmd == "-c" || cmd == "-c-only") {
                fs = C_FILES;
                if (cmd2 == "-s" || cmd2 == "-skips") {
                    skipped_lines_print = true;
                } else return arg::unknown;
                if (cmd1 == "-g" || cmd1 == "graph") {
                    return arg::cli_graph;
                } else return arg::unknown;
            } else return arg::unknown;
        }
    }
    else if (argc == 5) {
        str cmd1 = argv[2];
        str cmd2 = argv[3];
        str cmd3 = argv[4];
        if (cmd == "-c" || cmd == "-c-only") {
            fs = C_FILES;
            if (cmd1 == "-g" || cmd1 == "-graph") {
                if (cmd2 == "-e" || cmd2 == "-ext" || cmd2 == "-external") {
                    search_path = cmd3;
                }
                return arg::cli_graph;
            } else if (cmd1 == "-s" || cmd1 == "-skips") {
                skipped_lines_print = true;
                return arg::cli_default;
            } else return arg::unknown;
        } else if (cmd == "-g" || cmd == "-graph") {
            if (cmd1 == "-s" || cmd1 == "-skips") {
                skipped_lines_print = true;
                if (cmd2 == "-e" || cmd2 == "-ext" || cmd2 == "-external") {
                    search_path = cmd3;
                }
                return arg::cli_graph;
            } else return arg::unknown;
        } else return arg::unknown;
    }
    return arg::unknown;
}

void printHelp() {
    std::string helpMsg =
        "Usage: "
        "./dirloc [--gui] [options] [-e | -ext | -external <filepath>]\n"
        " -e, -ext, -external\tSets the directory from which DIRLOC\n"
        "\t\t\twill read files; (default is current directory)\n"
        " -c, -c-only\t\tReads only from C files (*.c, *.h)\n"
        " -g, -graph\t\tDisplays ASCII-style graphs to show relative\n"
        "\t\t\tsizes of the queried files\n"
        " -s, -skips\t\tPrints the # of skipped (undesireable) lines\n"
        "\t\t\talongside each file, the total for each\n"
        "\t\t\tfiletype, and the overall total\n"
        " --gui\t\t\tLaunches the GUI version of the app;\n\t\t\tcan\'t be used with other options\n"
        " --v, --version\t\tPrints the version of DIRLOC and exits\n"
        " --h, --help\t\tPrints this help message and exits\n"
        "\nOptions MUST be in alphabetical order (except for the external\n"
        "directory option, which must be placed at the end of the line)\n"
    ;

    std::cout << helpMsg;
}

void printVersion() {
    std::string versionMsg =
        "************************************\n"
        "* DIRLOC (Directory Lines Of Code) *\n"
        "************************************\n"
        "*    v0.1 (by ctrl-j on GitHub)    *\n"
        "************************************\n"
    ;

    std::cout << versionMsg;
}

int startCLI(int argc, char *argv[], DIRLOC_FLAG f) {
    QCoreApplication a(argc, argv);

    dirloc *d = new dirloc(search_path, f, fs, skipped_lines_print);

    delete d;
    return 0;
}

int startGUI(int argc, char *argv[]) {
    QApplication a(argc, argv);

    MainWindow *mw = new MainWindow(600, 400);
    int result = a.exec();

    delete mw;
    return result;
}

int main(int argc, char *argv[])
{
    dirloc *d = nullptr;
    MainWindow *mw = nullptr;
    int result = 0;

    // TODO: clean/split up the hashArgString function
    // TODO: add support for multiple directories with the -e switch

    switch(hashArgString(argc, argv)) {
    case arg::cli_default:
        return startCLI(argc, argv, CLI_NONE);

    case arg::cli_graph:
        return startCLI(argc, argv, CLI_GRAPH);

    case arg::gui:
        return startGUI(argc, argv);

    case arg::help:
        printHelp();
        break;

    case arg::version:
        printVersion();
        break;

    default:
        std::cout << "*** DIRLOC: UNKNOWN ARGUMENT \"" << argv[1] << "\" ***\n";
        printHelp();
        return 1;
    }

    return 0;
}
