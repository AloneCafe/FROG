#ifndef __BYTE_CODE_RELOCATOR_HEADER__
#define __BYTE_CODE_RELOCATOR_HEADER__

#include <vector>
#include <string>
#include <unordered_map>

class AddrLocateTable {
    friend class ByteCodeRelocator;
protected:
    std::unordered_map<std::string, size_t> _map;

public:
    virtual std::pair<bool, size_t> getOffset(const std::string & symbolName) const;
    
    virtual bool setOffset(const std::string & symbolName, size_t offset);
};

class AddrRelocateTable {
    friend class ByteCodeRelocator;
private:
    std::unordered_multimap<std::string, size_t> _map;

public:
    virtual std::pair<bool, size_t> getOffset(const std::string & symbolName) const;
    
    virtual void setOffset(const std::string & symbolName, size_t offset);
    
};

class ByteCodeRelocator {
public:
    static bool relocateStatic(std::vector<char> & bytesStatic,
            const AddrLocateTable & altStatic, AddrRelocateTable & artStatic);
    
    static bool relocateFuncs(std::vector<char> & bytesFuncs,
            const AddrLocateTable & altFuncs, AddrRelocateTable & artFuncs);
};

#endif
