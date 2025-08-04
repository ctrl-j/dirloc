#ifndef DIRLOC_HH
#define DIRLOC_HH

#include <cstdint>
#include <filesystem>
#include <QRegularExpression>
#include <vector>

enum DIRLOC_FLAG {
    CLI_NONE=0, CLI_GRAPH=1
};

enum FILESET {
    CPP_FILES=0, C_FILES=1
};

typedef std::string str;
typedef std::filesystem::path fpath;
typedef std::vector<std::string> strvec;
typedef std::vector<std::filesystem::path> fpvec;

// {filepath, extension, lines of code}
typedef std::tuple<fpath, str, uint32_t, uint32_t> fpext;
typedef std::vector<fpext> fpextvec;

// {lines of code, filename}
typedef std::tuple<uint32_t, str, uint32_t> fileloc;
typedef std::vector<fileloc> flvec;

class dirloc
{
private:
    DIRLOC_FLAG mode;
    FILESET fileset;
    strvec exts;

    fpextvec fpaths;
    fpath searchPath;
    uint64_t tloc;
    uint64_t tskips;

    bool print_skips;
    int graph_limit;

    static QRegularExpression rgxNewline;
    static QRegularExpression rgxOBrace;
    static QRegularExpression rgxCBrace;
    static QRegularExpression rgxCBraceSemi;

    strvec FILESET_C = {
        ".c", ".h"
    };

    strvec FILESET_CPP = {
        ".C", ".cc", ".cpp", ".cxx", ".ixx", ".cppm",
        ".h", ".hh", ".H", ".hpp", ".hxx"
    };

public:
    dirloc(str _searchPath, DIRLOC_FLAG _mode,
           FILESET _fileset, bool _print_skips);

    void initRegex();
    void findFiles();
    int openFiles();
    std::pair<int64_t, int64_t> readLines(std::ifstream &ifs);

    void printResults();
    void printLOCFiletypes();
    void printGraphs();
};

#endif // DIRLOC_HH
