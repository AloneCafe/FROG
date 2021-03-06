#include "ByteCodeRelocator.hpp"

#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>

std::pair<bool, uint32_t> AddrLocateTable::getOffset(const std::string & symbolName) const {
    auto it = _map.cbegin();
    if ((it = _map.find(symbolName)) != _map.cend()) {
        return std::make_pair(true, it->second);
    }
    return std::make_pair(false, 0);
}

bool AddrLocateTable::setOffset(const std::string & symbolName, uint32_t offset) {
    auto it = _map.insert(std::make_pair(symbolName, offset));
    return it.second;
}

std::pair<bool, uint32_t>
AddrRelocateTable::getOffset(const std::string & symbolName) const {
    auto it = _map.cbegin();
    if ((it = _map.find(symbolName)) != _map.cend()) {
        return std::make_pair(true, it->second);
    }
    return std::make_pair(false, 0);
}

void AddrRelocateTable::setOffset(const std::string & symbolName, uint32_t offset) {
    auto it = _map.insert(std::make_pair(symbolName, offset));
}

bool ByteCodeRelocator::relocateStatic(
        std::vector<char> & bytesStatic,
        const AddrLocateTable & altStatic,
        AddrRelocateTable & artStatic)
{
    // 先重定位静态区块
    for (const auto & e : altStatic._map) {
        const std::string & symbolName = e.first;
        uint32_t dwReloc = e.second;
        
        // 遍历重定位表, 找到符号替换其偏移位置的字节码 (DW)
        //std::pair<bool, uint32_t> pair;
        auto it = artStatic._map.cbegin();
        while ((it = artStatic._map.find(symbolName)) != artStatic._map.cend()) {
            // 重定位完成后, 删除此条重定位信息
            uint32_t offset = it->second;
            for (uint32_t i = 0; i < dwReloc; ++i) {
                bytesStatic[offset + i] = ((char *)&dwReloc)[i];
            }
            artStatic._map.erase(it);
        }
        
    }
    // 检查重定位表剩余, 如果存在剩余, 则说明存在无法定位符号
    if (!artStatic._map.empty()) {
        for (const auto & e : artStatic._map) {
            std::cerr << "~ 无法定位的符号 " << e.first << ", 位于字节码的静态区块偏移 " << e.second << " 处";
        }
        return false;
    }
    
    return true;
}

bool ByteCodeRelocator::relocateFuncs(
        std::vector<char> & bytesFuncs,
        const AddrLocateTable & altFuncs,
        AddrRelocateTable & artFuncs)
{
    // 再重定位非静态区块
    for (const auto & e : altFuncs._map) {
        const std::string & symbolName = e.first;
        uint32_t dwReloc = e.second;
        
        // 遍历重定位表, 找到符号替换其偏移位置的字节码 (DW)
        //std::pair<bool, uint32_t> pair;
        auto it = artFuncs._map.cbegin();
        while ((it = artFuncs._map.find(symbolName)) != artFuncs._map.cend()) {
            // 重定位完成后, 删除此条重定位信息
            uint32_t offset = it->second;
            for (uint32_t i = 0; i < 4; ++i) {
                bytesFuncs[offset + i] = ((char *)&dwReloc)[i];
            }
            artFuncs._map.erase(it);
        }
        
    }
    // 检查重定位表剩余, 如果存在剩余, 则说明存在无法定位符号
    if (!artFuncs._map.empty()) {
        for (const auto & e : artFuncs._map) {
            std::cerr << "~ 无法定位的符号 " << e.first << ", 位于字节码的函数 (非静态) 区块偏移 " << e.second << " 处";
        }
        return false;
    }
    
    return true;
}