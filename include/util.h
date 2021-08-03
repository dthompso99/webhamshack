#ifndef UTIL_H_
#define UTIL_H_

#include <string>

class Util {
    public:
        static bool saveFile(std::string filename, std::string data);
        static std::string readFile(std::string filename, std::string def);
        static std::vector<std::string> listTTY();
    private:

};
#endif // UTIL_H_