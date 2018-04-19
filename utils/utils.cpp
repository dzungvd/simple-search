#include "utils.h"
#include <iostream>
#include <fstream>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem.hpp>

Utils::Utils()
{

}

std::string Utils::convertToBase64(const unsigned char *input, size_t input_len) {

    if (input == NULL) {
        return std::string ("");
    }

    size_t b64_maxlen = sodium_base64_ENCODED_LEN(input_len, sodium_base64_VARIANT_ORIGINAL);
    //encode len = string_length + '\0' character

    char* b64 = new char [b64_maxlen];

    sodium_bin2base64(b64, b64_maxlen,
                      input, input_len,
                      sodium_base64_VARIANT_ORIGINAL);

    //b64_maxlen is string length plus '\0' char => discard the '\0' char before
    //puting it to result string
    std::string result (b64, b64_maxlen - 1);

    delete[] b64;
    return result;
}

std::string Utils::convertFromB64ToBin(const char *input, unsigned long long input_len) {
    if (input == NULL) {
        return std::string ("");
    }
    size_t bin_maxlen = input_len * 2;
    char* bin = new char [bin_maxlen];
    size_t bin_len = 0;
    if (sodium_base642bin(reinterpret_cast<unsigned char*> (bin), bin_maxlen,
                          input, input_len,
                          NULL, &bin_len,
                          NULL, sodium_base64_VARIANT_ORIGINAL) != 0) {
        delete[] bin;
        return std::string ("");
    }
    std::string result (bin, bin_len);
    delete[] bin;
    return result;
}

bool Utils::createFolder (std::string folderPath) {
    //dont continuous process if exists before
    if (Utils::isExists(folderPath)) {
        std::cerr << "Util::createFolder " << folderPath << " was exists" << std::endl;
        return true;
    }

    boost::filesystem::path path(folderPath);
    return boost::filesystem::create_directory(path);
}

bool Utils::createFile (std::string filePath) {
    // dont continuous process if exists before
    if (Utils::isExists(filePath)) {
        std::cerr << "Util::createFile " << filePath << " was exists" << std::endl;
        return true;
    }

    std::ofstream of(filePath);
    bool status = of.is_open();
    of.close();
    return status;
}

bool Utils::isExists(std::string path) {
    boost::filesystem::path fp(path);

    try {
        if (boost::filesystem::exists(fp))
            return true;
    } catch (boost::filesystem::filesystem_error &e) {
        std::cerr << e.what() << std::endl;
    }

    return false;
}
