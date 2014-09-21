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

// Function prototypes.
void process(const std::string&);

/**
 * @brief Gets the size of a file in bytes.
 * @param[in] path Path of the file to check.
 * @returns Size of the file in bytes.
 */
unsigned long get_file_size(const std::string& path) {
  std::ifstream file(path.c_str(), std::ios::binary | std::ios::ate);
  return unsigned(long(file.tellg()));
}

/**
 * @brief Converts a string to hex.
 * @param[in] str String to convert to hex.
 * @returns A string of hexadecimal characters.
 */
std::string convert_to_hex(const unsigned char* str, const unsigned int length) {
  static const char* hex = "0123456789ABCDEF";
  std::ostringstream ss;
  for(unsigned long i = 0; i < length; ++i) {
    // Gets the character.
    const unsigned char c = str[i];

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
  static unsigned char hash[SHA_DIGEST_LENGTH];
  ::memset(hash, 0, SHA_DIGEST_LENGTH);

  unsigned long length = get_file_size(path);
  if(length > 0) {
    int fd = ::open(path.c_str(), O_RDONLY);
    if(fd > 0) {
      // Map the file into memory.
      void* data = ::mmap(nullptr, length, PROT_READ, MAP_FILE | MAP_SHARED, fd, 0);
      if(data) {
        // Use OpenSSL to generate a SHA1 hash for the mapped file.
        ::SHA1(reinterpret_cast<const unsigned char*>(data), length, hash);

        // Unmap the file.
        ::munmap(data, length);
      }

      else {
        std::cerr << "* Error (" << errno << ") when mapping file: " << path << std::endl;
      }

      // Close the file.
      ::close(fd);
    }

    else {
      std::cerr << "* Error (" << errno << ") when opening file: " << path << std::endl;
    }
  }

  // Return the hash as a hex string.
  return convert_to_hex(hash, SHA_DIGEST_LENGTH);
}

/**
 * @brief Processes an individual file.
 * @param[in] path Path of file.
 */
void process_file(const std::string& path) {
  std::cout << file_hash(path) << " " << path << std::endl;
}

/**
 * @brief Returns whether the path is a directory.
 * @param[in] path Path to check.
 * @returns `true` if the path is a directory; otherwise `false`.
 */
bool is_directory(const std::string& path) {
  struct stat st;
  ::lstat(path.c_str(), &st);
  return S_ISDIR(st.st_mode);
}

/**
 * @brief Returns whether the path is a regular file.
 * @param[in] path Path to check.
 * @returns `true` if the path is a regular file; otherwise `false`.
 */
bool is_file(const std::string& path) {
  struct stat st;
  ::lstat(path.c_str(), &st);
  return S_ISREG(st.st_mode);
}

/**
 * @brief Processes all files recursively found under a directory.
 * @param[in] path Path to recurse.
 */
void process_directory(const std::string& path) {
  DIR* dir = ::opendir(path.c_str());
  if(dir == nullptr) {
    std::cerr << "* Error (" << errno << ") when opening directory: " << path << std::endl;
    return;
  }

  struct dirent* entry = nullptr;
  while((entry = ::readdir(dir)) != nullptr) {
    std::string sub_dir(entry->d_name);

    // Don't process the "." and ".." directories.
    if(sub_dir != "." && sub_dir != "..") {
      // Concatinate the path.
      std::ostringstream new_path;
      new_path << path << "/" << sub_dir;

      // Process the path.
      process(new_path.str());
    }
  }

  ::closedir(dir);
}

void process(const std::string& path) {
  if(is_directory(path)) {
    process_directory(path);
  }
  else if(is_file(path)) {
    process_file(path);
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
  process(argv[1]);
  return 0;
}
