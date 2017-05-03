#include "bookmark.hh"
#include "options.hh"
#include "duplication.hh"
#include "file.hh" // readFileIntoString

#include <iostream>   // cout, endl, ostream
#include <cstring>    // strcmp
#include <string>
#include <vector>

using std::cout;
using std::endl;

static const char* order(int);

int                               Bookmark::totalNrOfLines = 0;
std::vector<Bookmark::FileRecord> Bookmark::fileRecords;
std::string                       Bookmark::totalString;

/**
 * Reports one instance of duplication and optionally prints the duplicated
 * string.
 */
void Bookmark::report(const Duplication& duplication,
                      int                instanceNr,
                      const Options&     options) const
{
    static int count = 0;

    if (instanceNr == 1)
        ++count;

    cout << *this << ":Duplication " << count << " (" << instanceNr
         << order(instanceNr) << " instance";
    int nrOfLines = details(duplication.longestSame, COUNT_LINES,
                            options.wordMode);
    if (instanceNr == 1)
        cout << ", " << duplication.longestSame << " characters, "
             << nrOfLines << " line" << (nrOfLines == 1 ? "" : "s");
    else
        totalNrOfLines += nrOfLines;

    cout << ")." << endl;
    if (options.isVerbose && instanceNr == duplication.instances)
        details(duplication.longestSame, PRINT_LINES, options.wordMode);
}

// Returns the correct suffix for strings like 1st, 2nd, 3rd, 4th, etc.
static const char* order(int nr)
{
    if      (nr % 10 == 1 && nr % 100 != 11) return "st";
    else if (nr % 10 == 2 && nr % 100 != 12) return "nd";
    else if (nr % 10 == 3 && nr % 100 != 13) return "rd";
    else                                     return "th";
}

bool Bookmark::operator<(const Bookmark& another) const // Used in sorting.
{
    return strcmp(another.processed, this->processed) < 0;
}

int Bookmark::nrOfSame(Bookmark b) const
{
    int index = 0;
    for (; this->processed[index] == b.processed[index]; ++index)
        // The characters are equal so we only have to check one of them.
        if (this->processed[index] == SPECIAL_EOF)
            break;
    return index;
}

/**
 * Used for optimization purposes. By comparing strings backwards we can
 * find out quickly if the two strings are not equal in the given number of
 * characters and move on to the next comparison.
 */
bool
Bookmark::sameAs(Bookmark b, int nrOfCharacters, const char* end) const
{
    if (&this->processed[nrOfCharacters] >= end ||
        &b.processed[nrOfCharacters] >= end)
    {
        return false;
    }

    for (int i = nrOfCharacters; i >= 0; --i)
        if (this->processed[i] != b.processed[i])
            return false;

    return true;
}

void Bookmark::addFile(const std::string& fileName)
{
    totalString += readFileIntoString(fileName.c_str());
    fileRecords.push_back(FileRecord(fileName, totalString.length()));
}

int Bookmark::details(int        processedLength,
                      DetailType detailType,
                      bool       wordMode) const
{
    const char* orig = totalString.c_str() + this->original;
    if (not wordMode)
    {
        while (orig > totalString.c_str() && *orig != '\n')
            --orig; // to include leading whitespace in printout
        ++orig;
    }
    int  count     = 1;
    bool blankLine = true;
    for (int pi = 0; pi < processedLength; ++pi, ++orig)
        for (; *orig != 0 && *orig != SPECIAL_EOF; ++orig)
        {
            if (detailType == PRINT_LINES)
                cout << *orig;
            else if (*orig == '\n')
            {
                if (not blankLine)
                {
                    count++;
                    blankLine = true;
                }
            }
            else if (not isspace(*orig))
                blankLine = false;
            // In word mode, a space in the processed text means any kind of
            // space, so we can not continue to search for an exact match.
            if (*orig == this->processed[pi] ||
                (isspace(this->processed[pi]) && isspace(*orig)))
            {
                break;
            }
        }
    if (detailType == PRINT_LINES)
        cout << endl;
    return count;
}

int Bookmark::lineNr(int offset, int index)
{
    const int start = (index == 0) ? 0 : fileRecords[index - 1].endIx;
    int result = 1;
    for (int i = start; i < offset; ++i)
        if (Bookmark::totalString[i] == '\n')
            ++result;
    return result;
}

std::ostream& operator<<(std::ostream& os, const Bookmark& b)
{
    int recIx = 0;
    while (Bookmark::fileRecords[recIx].endIx <= b.original)
        ++recIx;

    os << Bookmark::fileRecords[recIx].fileName << ":"
       << Bookmark::lineNr(b.original, recIx);
    return os;
}
