// Standard C headers.
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>

// System C headers.
#include <sys/mman.h>
#include <sys/stat.h>

// Standard C++ headers.
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

// OpenSSL headers.
#include <openssl/sha.h>

/**
 * @brief Gets the size of a file in bytes.
 * @param[in] path Path of the file to check.
 * @returns Size of the file in bytes.
 */
unsigned long get_file_size(const std::string& path) {
  std::ifstream file(path.c_str(), std::ios::binary | std::ios::ate);
  return (unsigned long)(file.tellg());
}

/**
 * @brief Converts a string to hex.
 * @param[in] str String to convert to hex.
 * @returns A string of hexadecimal characters.
 */
std::string convert_to_hex(const std::string& str) {
  static const char* hex = "0123456789ABCDEF";
  std::ostringstream ss;
  for(unsigned long i = 0; i < str.length(); ++i) {
    // Gets the character.
    const char c = str[i];

    // Computes the nibbles.
    const unsigned char high_nibble = (c & 0xF0) >> 4;
    const unsigned char low_nibble =  c & 0x0F;

    // Uses the nibbles to find the character in the hex lookup table.
    ss << hex[high_nibble];
    ss << hex[low_nibble];
  }
  return ss.str();
}

/**
 * @brief Gets the SHA1 hash of a file.
 */
std::string file_hash(const std::string& path) {
  unsigned char hash[SHA_DIGEST_LENGTH];
  ::memset(hash, SHA_DIGEST_LENGTH, 0);

  unsigned long length = get_file_size(path);
  int fd = ::open(path.c_str(), O_RDONLY);

  // Map the file into memory.
  void* base = ::mmap(nullptr, length, PROT_READ, MAP_FILE | MAP_SHARED, fd, 0);
  if(base) {
    // Use OpenSSL to generate a SHA1 hash for the mapped file.
    ::SHA1(reinterpret_cast<const unsigned char*>(base), length, hash);

    // Unmap the file.
    ::munmap(base, length);
  }

  else {
    std::cerr << "* Error (" << errno << ") when mapping file: " << path << std::endl;
  }

  // Close the file.
  ::close(fd);

  // Return the hash as a hex string.
  return convert_to_hex(std::string(reinterpret_cast<const char*>(hash), SHA_DIGEST_LENGTH));
}

/**
 * @brief Processes all files recursively found under a directory.
 * @param[in] path Path to recurse.
 * @param[in] func Function to invoke on each file found under the directory.
 */
void process_file(const std::string& path, std::function<void(std::string)> func) {
  struct stat st;
  lstat(path.c_str(), &st);
  if(S_ISDIR(st.st_mode)) {
    DIR* dir = opendir(path.c_str());
    if(dir == nullptr) {
      std::cerr << "* Error (" << errno << ") when opening directory: " << path << std::endl;
      return;
    }
    struct dirent* entry = nullptr;
    while((entry = readdir(dir)) != nullptr) {
      std::string sub_dir(entry->d_name);
      if(sub_dir != "." && sub_dir != "..") {
        std::ostringstream new_path;
        new_path << path << "/" << sub_dir;
        process_file(new_path.str(), func);
      }
    }
    closedir(dir);
  }
  else {
    func(path);
  }
}

/**
 * @brief Main entry point into the utility.
 * @param[in] argc Number of command line arguments
 * @param[in] argv Command line argument values.
 *
 * Note that `argv[0]` is the command used to launch the utility.
 */
int main(int argc, char** argv) {
  (void)(argc);
  (void)(argv);
  process_file(argv[1], [](const std::string& path) {
    std::cout << file_hash(path) << " " << path << std::endl;
  });
  return 0;
}
