#include "dirloc.hh"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>

QRegularExpression rgxNewline;
QRegularExpression rgxOBrace;
QRegularExpression rgxCBrace;
QRegularExpression rgxCBraceSemi;

const std::vector<str> SKIP = {
    "{", "}", "};"
};

void dirloc::initRegex() {
    // TODO ... ... ... ... ... ... ...

    // Matches "undesireable" lines which end in the
    // strings found in the "SKIP" array
}

dirloc::dirloc(str _searchPath, DIRLOC_FLAG _mode, FILESET _fileset, bool _print_skips) {
    if (_searchPath.empty()) {
        std::cout << "*** ERROR: search path is empty! ***\n";
        return;
    }

    // Check if search path is relative; convert to absolute if so
    fpath relCheck = _searchPath;
    if (relCheck.is_relative()) {
        std::error_code e;
        searchPath = std::filesystem::canonical(relCheck, e);
        if (static_cast<bool>(e)) {
            std::cout << "*** ERROR: could not convert relative path to absolute path! ***\n";
            return;
        }
    } else searchPath = _searchPath;

    if (!std::filesystem::exists(searchPath)) {
        std::cout << "*** ERROR: search path does not exist! ***\n";
        return;
    }
    if (!std::filesystem::is_directory(searchPath)) {
        std::cout << "*** ERROR: search path is not a directory! ***\n";
        return;
    }

    // Set members
    tloc = 0;
    tskips = 0;
    graph_limit = 50;
    mode = _mode;
    fileset = _fileset;
    print_skips = _print_skips;

    // File extensions to search for
    if (fileset == C_FILES) {
        exts = FILESET_C;
    } else if (fileset == CPP_FILES) {
        exts = FILESET_CPP;
    } else {
        exts = strvec(0);
    }

    // Start the program in user-selected "mode"
    if (mode == CLI_NONE || mode == CLI_GRAPH) {
        initRegex();
        findFiles();
        if (openFiles()) {
            std::cout << "Aborting dirloc...\n";
            return;
        }

        if (fpaths.size() == 0) {
            std::cout << "NO FILES FOUND! Exiting...\n";
            return;
        }

        printResults();
    }
    // TODO: add support for the GUI mode
    else {
        return;
    }
}

int dirloc::openFiles() {
    // For each file in "fpaths",
    for (fpext &fpel : fpaths) {
        // Get the filepath
        fpath fp = std::get<0>(fpel);

        // Open the file as readonly
        std::ifstream code;
        code.open(fp, std::ios_base::in);
        if (!code.is_open()) {
            std::cout << "*** ERROR: failed while attempting to read "
                      << fp << " ***\n";
            return 1;
        }

        // Read the actual lines of the file, skipping undesirable ones
        std::pair<int64_t, int64_t> locnskip = readLines(code);
        uint64_t loc = std::get<0>(locnskip);
        uint64_t skipd = std::get<1>(locnskip);

        if (loc < 0) {
            std::cout << "*** ERROR: failed while attempting to read "
                      << fp << " ***\n";
            return 1;
        }

        // Set the tuple member for this file's LOCs and skips
        std::get<2>(fpel) = loc;
        std::get<3>(fpel) = skipd;

        // Add LOC to "tloc"
        tloc += loc;
    }

    return 0;
}

std::pair<int64_t, int64_t> dirloc::readLines(std::ifstream &ifs) {
    str line;
    int64_t loc = 0;
    int64_t skips = 0;

    // Read until end of file
    while (!ifs.eof()) {
        std::getline(ifs, line);

        // Skip empty lines
        if (line.empty()) continue;

        // Strip the string of leading/trailing whitespace
        line = std::regex_replace(line,
                                  std::regex(R"(^\s+|\s+$)"),
                                  "");

        // Skip any comment lines
        if (line.starts_with("//") ||
            line.starts_with("/*") ||
            line.starts_with("*")) {
            skips++;
            continue;
        }

        // Skip any undesireable tokens, do not add to total LOC
        bool match = false;
        for (const str &s : SKIP) {
            if (line.ends_with(s)) {
                skips++;
                match = true;
                break;
            }
        }
        if (match == false) loc++;
    }

    return std::pair<int64_t, int64_t>(loc, skips);
}

void dirloc::findFiles() {
    // Exit if no extensions to search for (invalid fileset arg)
    if (exts.size() == 0) {
        std::cout << "*** ERROR (findFiles): string vector of "
                     "file extensions is EMPTY! ***\n";
        return;
    }

    // Print search directory
    std::cout << "(Search dir: " << searchPath << ")\n";

    // Check if file's extension is in "exts"
    // Add to vector if true
    fpaths = fpextvec(0);
    for (auto const &dir_entry :
            std::filesystem::
                directory_iterator(searchPath)) {

        fpath dfp = dir_entry.path();
        if (std::find(exts.begin(),
                      exts.end(),
                      dfp.extension())
                        != exts.end()) {

            fpext found = {dfp, dfp.extension(), 0, 0};
            fpaths.push_back(found);
        }
    }
}

void dirloc::printResults() {
    // Print the "tokens" being ignored
    std::cout << "Skipping lines that end with ";
    for (int i = 0; i < SKIP.size() - 1; i++) {
        std::cout << "\"" << SKIP[i] << "\", ";
    }
    std::cout << "\"" << SKIP[SKIP.size() - 1] << "\"\n\n";


    if (mode == CLI_NONE) printLOCFiletypes();
    else if (mode == CLI_GRAPH) printGraphs();
    else {
        std::cout << "*** ERROR (printResults): invalid printing `mode` value - "
                  << mode << " ***\n";
        return;
    }
}

void dirloc::printGraphs() {
    // For each file in "fpaths",
    for (str &e : exts) {
        flvec toPrint(0);
        // Check if it matches current extension
        for (fpext &fp : fpaths) {
            if (std::get<1>(fp) == e) {
                str fname = std::get<0>(fp).filename();
                uint32_t loc = std::get<2>(fp);
                uint32_t skips = std::get<3>(fp);
                tskips += skips;
                toPrint.push_back(fileloc(loc, fname, skips));
            }
        }
        // Only print this filetype if some were found
        if (toPrint.size() > 0) {
            std::cout << e << " files:\n";
            uint64_t tlocExt = 0;
            uint64_t skipsExt = 0;

            // Find the file with most LOC and most skips
            uint32_t maxLOC = 0;
            uint32_t maxSkips = 0;
            fileloc fileMostLOC;
            fileloc fileMostSkips;
            for (int i = 0; i < toPrint.size(); i++) {
                fileloc p = toPrint[i];
                if (std::get<0>(p) > maxLOC) {
                    maxLOC = std::get<0>(p);
                    fileMostLOC = p;
                }
                if (std::get<2>(p) > maxSkips) {
                    maxSkips = std::get<2>(p);
                    fileMostSkips = p;
                }
            }

            // For each file to print,
            for (int i = 0; i < toPrint.size(); i++) {
                fileloc p = toPrint[i];

                // Print the filename, LOC, and (optionally) skips
                std::cout << "\t" << std::get<1>(p)
                          << " [" << std::get<0>(p)
                          << " LOC";
                if (print_skips) {
                    std::cout << ", " << std::get<2>(p) << " skips]\n";
                } else {
                    std::cout << "]\n";
                }

                // Print graph for # of LOC, scaled based on highest LOC count
                str gs = "\t\t";
                if (std::get<1>(p) == std::get<1>(fileMostLOC)) {
                    gs.append(graph_limit, '#');
                } else {
                    uint32_t loc = std::get<0>(p);
                    int len = ((long double)loc / (long double)maxLOC) * 50.0f;
                    gs.append(len, '#');
                }
                std::cout << gs << "\n";

                // Print graph for # of skips, scaled based on highest skip count
                if (print_skips) {
                    str gss = "\t\t";
                    if (std::get<1>(p) == std::get<1>(fileMostSkips)) {
                        gss.append(graph_limit, '$');
                    } else {
                        uint32_t skips = std::get<2>(p);
                        int len = ((long double)skips / (long double)maxSkips) * 50.0f;
                        gss.append(len, '$');
                    }
                    std::cout << gss << "\n";
                }
            }
        }
    }
}

void dirloc::printLOCFiletypes() {
    // For each file in "fpaths",
    uint64_t tskips = 0;
    for (str &e : exts) {
        flvec toPrint(0);

        // Check if it matches current extension
        for (fpext &fp : fpaths) {
            if (std::get<1>(fp) == e) {
                str fname = std::get<0>(fp).filename();
                uint32_t loc = std::get<2>(fp);
                uint32_t skips = std::get<3>(fp);
                tskips += skips;
                toPrint.push_back(fileloc(loc, fname, skips));
            }
        }
        // Only print this filetype if some were found
        if (toPrint.size() > 0) {
            std::cout << e << " files:\n";
            uint64_t tlocExt = 0;
            uint64_t skipsExt = 0;

            // Print each file's LOC, its filename, and skipped LOC
            for (fileloc &fl : toPrint) {
                std::cout << "\t"
                          << std::get<0>(fl)
                          << "\t"
                          << std::get<1>(fl);
                if (print_skips) {
                    std::cout << "\t[" << std::get<2>(fl) << " skipped]";
                }
                std::cout << "\n";

                // Add to running total for this filetype
                tlocExt += std::get<0>(fl);
                skipsExt += std::get<2>(fl);
            }
            // Print total LOC and skips for this filetype
            std::cout << "\t{" << tlocExt << " total ";
            if (print_skips) {
                std::cout << ", " << skipsExt << " skipped ";
            }

            std::cout <<  "in " << e << " files}\n";

        }
    }

    std::cout << "\nTOTAL LINES OF CODE: " << tloc << "\n";
    if (print_skips) {
        std::cout << "TOTAL SKIPPED LINES: " << tskips << "\n";
    }
}
