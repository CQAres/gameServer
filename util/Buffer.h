#pragma once
#include <string>

class Buffer
{
private:
    std::string buf;
public:
    Buffer();
    ~Buffer();
    /**
     * @brief 将指定字符串追加到缓冲区中
     * @brief 遍历字符串中的每个字符，直到遇到空字符或达到指定大小，将字符逐个添加到缓冲区。
     * @param _str 要追加的字符串
     * @param _size 字符串的最大长度 
     */
    void append(const char* _str, int _size);

    /**
     * @brief 获取缓冲区当前的大小
     * @return 返回缓冲区中存储数据的字节数
     */
    ssize_t size();

    /**
     * @brief 获取缓冲区当前的字符串
     * @return 返回缓冲区中的字符串
     */
    const char* c_str();

    /**
     * @brief 清空缓冲区
     */
    void clear();

    /**
    * @brief 从标准输入读取一行数据到缓冲区
    */
    void getline();

    /**
     * @brief 设置缓冲区当前的字符串
     * @param _buf 要设置的字符串
     */
    void setBuf(const char* _buf);
};