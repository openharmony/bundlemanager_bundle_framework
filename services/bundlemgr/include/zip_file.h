/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_ZIP_FILE_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_ZIP_FILE_H

#include <cstdint>
#include <map>
#include <string>
#include <unistd.h>

#include "ffrt.h"
#include "unzip.h"

namespace OHOS {
namespace AppExecFwk {
struct CentralDirEntry;
struct ZipEntry;
using ZipPos = ZPOS64_T;
using ZipEntryMap = std::map<std::string, ZipEntry>;
using BytePtr = Byte *;

// Local file header: descript in APPNOTE-6.3.4
//    local file header signature     4 bytes  (0x04034b50)
//    version needed to extract       2 bytes
//    general purpose bit flag        2 bytes
//    compression method              2 bytes  10
//    last mod file time              2 bytes
//    last mod file date              2 bytes
//    crc-32                          4 bytes
//    compressed size                 4 bytes  22
//    uncompressed size               4 bytes
//    file name length                2 bytes
//    extra field length              2 bytes  30
struct __attribute__((packed)) LocalHeader {
    uint32_t signature = 0;
    uint16_t versionNeeded = 0;
    uint16_t flags = 0;
    uint16_t compressionMethod = 0;
    uint16_t modifiedTime = 0;
    uint16_t modifiedDate = 0;
    uint32_t crc = 0;
    uint32_t compressedSize = 0;
    uint32_t uncompressedSize = 0;
    uint16_t nameSize = 0;
    uint16_t extraSize = 0;
};

// central file header
//    Central File header:
//    central file header signature   4 bytes  (0x02014b50)
//    version made by                 2 bytes
//    version needed to extract       2 bytes
//    general purpose bit flag        2 bytes  10
//    compression method              2 bytes
//    last mod file time              2 bytes
//    last mod file date              2 bytes
//    crc-32                          4 bytes  20
//    compressed size                 4 bytes
//    uncompressed size               4 bytes
//    file name length                2 bytes  30
//    extra field length              2 bytes
//    file comment length             2 bytes
//    disk number start               2 bytes
//    internal file attributes        2 bytes
//    external file attributes        4 bytes
//    relative offset of local header 4 bytes 46byte
struct __attribute__((packed)) CentralDirEntry {
    uint32_t signature = 0;
    uint16_t versionMade = 0;
    uint16_t versionNeeded = 0;
    uint16_t flags = 0;  // general purpose bit flag
    uint16_t compressionMethod = 0;
    uint16_t modifiedTime = 0;
    uint16_t modifiedDate = 0;
    uint32_t crc = 0;
    uint32_t compressedSize = 0;
    uint32_t uncompressedSize = 0;
    uint16_t nameSize = 0;
    uint16_t extraSize = 0;
    uint16_t commentSize = 0;
    uint16_t diskNumStart = 0;
    uint16_t internalAttr = 0;
    uint32_t externalAttr = 0;
    uint32_t localHeaderOffset = 0;
};

// end of central directory packed structure
//    end of central dir signature    4 bytes  (0x06054b50)
//    number of this disk             2 bytes
//    number of the disk with the
//    start of the central directory  2 bytes
//    total number of entries in the
//    central directory on this disk  2 bytes
//    total number of entries in
//    the central directory           2 bytes
//    size of the central directory   4 bytes
//    offset of start of central
//    directory with respect to
//    the starting disk number        4 bytes
//    .ZIP file comment length        2 bytes
struct __attribute__((packed)) EndDir {
    uint32_t signature = 0;
    uint16_t numDisk = 0;
    uint16_t startDiskOfCentralDir = 0;
    uint16_t totalEntriesInThisDisk = 0;
    uint16_t totalEntries = 0;
    uint32_t sizeOfCentralDir = 0;
    uint32_t offset = 0;
    uint16_t commentLen = 0;
};

// Data descriptor:
//    data descriptor signature       4 bytes  (0x06054b50)
//    crc-32                          4 bytes
//    compressed size                 4 bytes
//    uncompressed size               4 bytes
// This descriptor MUST exist if bit 3 of the general purpose bit flag is set (see below).
// It is byte aligned and immediately follows the last byte of compressed data.
struct __attribute__((packed)) DataDesc {
    uint32_t signature = 0;
    uint32_t crc = 0;
    uint32_t compressedSize = 0;
    uint32_t uncompressedSize = 0;
};

struct ZipEntry {
    ZipEntry() = default;
    explicit ZipEntry(const CentralDirEntry &centralEntry);
    ~ZipEntry() = default;  // for CodeDEX warning

    uint16_t compressionMethod = 0;
    uint16_t flags = 0;
    uint32_t uncompressedSize = 0;
    uint32_t compressedSize = 0;
    uint32_t localHeaderOffset = 0;
    uint32_t crc = 0;
    std::string fileName;
};

// zip file extract class for bundle format.
class ZipFile {
public:
    explicit ZipFile(const std::string &pathName, bool parallel = false);
    ~ZipFile();
    /**
     * @brief Open zip file.
     * @return Returns true if the zip file is successfully opened; returns false otherwise.
     */
    bool Open();
    /**
     * @brief Close zip file.
     */
    void Close();
    /**
     * @brief Set this zip content start offset and length in the zip file form pathName.
     * @param start Indicates the zip content location start position.
     * @param length Indicates the zip content length.
     */
    void SetContentLocation(ZipPos start, size_t length);
    /**
     * @brief Get all entries in the zip file.
     * @param start Indicates the zip content location start position.
     * @param length Indicates the zip content length.
     * @return Returns the ZipEntryMap object cotain all entries.
     */
    const ZipEntryMap &GetAllEntries() const;
    /**
     * @brief Has entry by name.
     * @param entryName Indicates the entry name.
     * @return Returns true if the ZipEntry is successfully finded; returns false otherwise.
     */
    bool HasEntry(const std::string &entryName) const;

    bool IsDirExist(const std::string &dir) const;

    /**
     * @brief Get entry by name.
     * @param entryName Indicates the entry name.
     * @param resultEntry Indicates the obtained ZipEntry object.
     * @return Returns true if the ZipEntry is successfully finded; returns false otherwise.
     */
    bool GetEntry(const std::string &entryName, ZipEntry &resultEntry) const;
    /**
     * @brief Get data relative offset for file.
     * @param file Indicates the entry name.
     * @param offset Indicates the obtained offset.
     * @param length Indicates the length.
     * @return Returns true if this function is successfully called; returns false otherwise.
     */
    bool GetDataOffsetRelative(const std::string &file, ZipPos &offset, uint32_t &length) const;
    /**
     * @brief Get data relative offset for file.
     * @param file Indicates the entry name.
     * @param dest Indicates the obtained ostream object.
     * @return Returns true if file is successfully extracted; returns false otherwise.
     */
    bool ExtractFile(const std::string &file, std::ostream &dest) const;
    /**
     * @brief Extract data from file parallelly.
     * @brief Get data relative offset for file.
     * @param file Indicates the entry name.
     * @param dest Indicates the obtained ostream object.
     * @return Returns true if file is successfully extracted; returns false otherwise.
     */
    bool ExtractFileParallel(const std::string &file, std::ostream &dest) const;

private:
    /**
     * @brief Check the EndDir object.
     * @param endDir Indicates the EndDir object to check.
     * @return Returns true if  successfully checked; returns false otherwise.
     */
    bool CheckEndDir(const EndDir &endDir) const;
    /**
     * @brief Parse the EndDir.
     * @return Returns true if  successfully Parsed; returns false otherwise.
     */
    bool ParseEndDirectory();
    /**
     * @brief Parse all Entries.
     * @return Returns true if successfully parsed; returns false otherwise.
     */
    bool ParseAllEntries();
    /**
     * @brief Get LocalHeader object size.
     * @param nameSize Indicates the nameSize.
     * @param extraSize Indicates the extraSize.
     * @return Returns size of LocalHeader.
     */
    size_t GetLocalHeaderSize(const uint16_t nameSize = 0, const uint16_t extraSize = 0) const;
    /**
     * @brief Get entry data offset.
     * @param zipEntry Indicates the ZipEntry object.
     * @param extraSize Indicates the extraSize.
     * @return Returns position.
     */
    ZipPos GetEntryDataOffset(const ZipEntry &zipEntry, const uint16_t extraSize) const;
    /**
     * @brief Check data description with specific file handler.
     * @param zipEntry Indicates the ZipEntry object.
     * @param localHeader Indicates the localHeader object.
     * @param file Indicates the file handler used to read data.
     * @return Returns true if successfully checked; returns false otherwise.
     */
    bool CheckDataDescInternal(const ZipEntry &zipEntry, const LocalHeader &localHeader, FILE *file) const;
    /**
     * @brief Check data description.
     * @param zipEntry Indicates the ZipEntry object.
     * @param localHeader Indicates the localHeader object.
     * @return Returns true if successfully checked; returns false otherwise.
     */
    bool CheckDataDesc(const ZipEntry &zipEntry, const LocalHeader &localHeader) const;
    /**
     * @brief Check coherency LocalHeader object with specific file handler.
     * @param zipEntry Indicates the ZipEntry object.
     * @param extraSize Indicates the obtained size.
     * @param file Indicates the file handler used to read data.
     * @return Returns true if successfully checked; returns false otherwise.
     */
    bool CheckCoherencyLocalHeaderInternal(const ZipEntry &zipEntry, uint16_t &extraSize, FILE *file) const;
    /**
     * @brief Check coherency LocalHeader object.
     * @param zipEntry Indicates the ZipEntry object.
     * @param extraSize Indicates the obtained size.
     * @return Returns true if successfully checked; returns false otherwise.
     */
    bool CheckCoherencyLocalHeader(const ZipEntry &zipEntry, uint16_t &extraSize) const;
    /**
     * @brief Unzip ZipEntry object to ostream.
     * @param zipEntry Indicates the ZipEntry object.
     * @param extraSize Indicates the size.
     * @param dest Indicates the obtained ostream object.
     * @param file Indicates the file handler used to read data.
     * @return Returns true if successfully Unzip; returns false otherwise.
     */
    bool UnzipWithStore(const ZipEntry &zipEntry, const uint16_t extraSize, std::ostream &dest, FILE *file) const;
    /**
     * @brief Unzip ZipEntry object to ostream.
     * @param zipEntry Indicates the ZipEntry object.
     * @param extraSize Indicates the size.
     * @param dest Indicates the obtained ostream object.
     * @param file Indicates the file handler used to read data.
     * @return Returns true if successfully Unzip; returns false otherwise.
     */
    bool UnzipWithInflated(const ZipEntry &zipEntry, const uint16_t extraSize, std::ostream &dest, FILE *file) const;
    /**
     * @brief Seek to Entry start with specific file handler.
     * @param zipEntry Indicates the ZipEntry object.
     * @param extraSize Indicates the extra size.
     * @param file Indicates the file handler used to read data.
     * @return Returns true if successfully Seeked; returns false otherwise.
     */
    bool SeekToEntryStartInternal(const ZipEntry &zipEntry, const uint16_t extraSize, FILE *file) const;
    /**
     * @brief Seek to Entry start.
     * @param zipEntry Indicates the ZipEntry object.
     * @param extraSize Indicates the extra size.
     * @return Returns true if successfully Seeked; returns false otherwise.
     */
    bool SeekToEntryStart(const ZipEntry &zipEntry, const uint16_t extraSize) const;
    /**
     * @brief Init zlib stream.
     * @param zstream Indicates the obtained z_stream object.
     * @return Returns true if successfully init; returns false otherwise.
     */
    bool InitZStream(z_stream &zstream) const;
    /**
     * @brief Read zlib stream.
     * @param buffer Indicates the buffer to read.
     * @param zstream Indicates the obtained z_stream object.
     * @param remainCompressedSize Indicates the obtained size.
     * @param file Indicates the file handler used to read data.
     * @return Returns true if successfully read; returns false otherwise.
     */
    bool ReadZStream(const BytePtr &buffer, z_stream &zstream, uint32_t &remainCompressedSize, FILE *file) const;
    /**
     * @brief Get file handler for parallel decompression.
     * @param resourceId Indicates the initial resource id, returns actual resource id by input parameter.
     */
    void GetFileHandler(int32_t &resourceId) const;
    /**
     * @brief Release file handler for parallel compression
     * @param resourceId Indicates the file handler id to release.
     */
    void ReleaseFileHandler(const int32_t resourceId) const;

private:
    bool isOpen_ = false;
    std::string pathName_;
    FILE *file_ = nullptr;
    // offset of central directory relative to zip file.
    ZipPos centralDirPos_ = 0;
    // this zip content start offset relative to zip file.
    ZipPos fileStartPos_ = 0;
    // this zip content length in the zip file.
    ZipPos fileLength_ = 0;
    EndDir endDir_;
    ZipEntryMap entriesMap_;
    
    bool parallel_ = false;
    const int32_t concurrency_ = sysconf(_SC_NPROCESSORS_ONLN);
    mutable std::vector<ffrt::mutex> mtxes_ = std::vector<ffrt::mutex> (concurrency_);
    std::vector<FILE *> files_;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_ZIP_FILE_H
