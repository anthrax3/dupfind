#ifndef OPTIONS_HH
#define OPTIONS_HH

#include <string>
#include <vector>

class Options
{
public:
    enum TotalReport { NO_TOTAL, RESTRICTED_TOTAL, UNRESTRICTED_TOTAL };
    enum ExtFlagMode { SHOW_EXT_FLAGS, HIDE_EXT_FLAGS };

    int                      nrOfWantedReports;
    bool                     isVerbose;
    TotalReport              totalReport;
    int                      minLength;
    int                      proximityFactor;
    bool                     wordMode;
    std::vector<std::string> foundFiles;
    std::vector<std::string> excludes;

    Options();

    void parse(int argc, char* argv[]);

    static void printUsageAndExit(ExtFlagMode extFlagMode, int exitCode);

private:
    int processFlag(int i, int argc, char* argv[]);
    void processFileName(const std::string& arg);
    void findFiles(const std::string& name, const std::string& ending);
};

#endif
