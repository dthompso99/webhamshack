#include <iostream>
#include <fstream>
#include <experimental/filesystem>
#include "../include/util.h"

//namespace fs = std::experimental::filesystem;

bool Util::saveFile(std::string filename, std::string content) {
    std::ofstream f;
    f.open(filename);
    f << content;
    f.close();
    return true;
}

std::string Util::readFile(std::string filename, std::string def) {
    std::string ret;
    std::ifstream f(filename);
    if (f.is_open()) {
        f >> ret;
        return ret;
    } else {
        return def;
    }
}

std::vector<std::string> Util::listTTY() {
    std::vector<std::string> port_names;

    std::experimental::filesystem::path p("/dev/serial/by-id");
    try {
      if (!exists(p)) {
        throw std::runtime_error(p.generic_string() + " does not exist");
      } else {
        for (auto &de : std::experimental::filesystem::directory_iterator(p)) {
          if (is_symlink(de.symlink_status())) {
            std::experimental::filesystem::path symlink_points_at = read_symlink(de);
            std::experimental::filesystem::path canonical_path = std::experimental::filesystem::canonical(symlink_points_at, p);
            port_names.push_back(canonical_path.generic_string());
          }
        }
      }
    } catch (const std::experimental::filesystem::filesystem_error &ex) {
      std::cout << ex.what() << std::endl;
      throw ex;
    }
    //std::sort(port_names.begin(), port_names.end());
    return port_names;
}