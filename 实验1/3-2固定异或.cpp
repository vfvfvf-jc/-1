#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//将十六进制字符转换为对应的数值
int hex_char_to_value(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

//将十六进制字符串转换为字节数组
int hex_to_bytes(const char *hex, unsigned char *bytes, int max_len) {
    // 获取输入字符串长度
    int len = strlen(hex);
    
    // 检查长度是否为偶数（每个字节需要2个十六进制字符）
    if (len % 2 != 0) {
        printf("错误: 十六进制字符串长度必须为偶数\n");
        return -1;
    }
    
    int byte_len = len / 2;
    
    // 检查输出缓冲区是否足够
    if (byte_len > max_len) {
        printf("错误: 输入数据过长\n");
        return -1;
    }
    
    // 遍历每对十六进制字符
    for (int i = 0; i < byte_len; i++) {
        // 获取高位和低位字符
        int high = hex_char_to_value(hex[i * 2]);      // 第一个字符（高4位）
        int low = hex_char_to_value(hex[i * 2 + 1]);   // 第二个字符（低4位）
        
        // 检查字符有效性
        if (high == -1 || low == -1) {
            printf("错误: 无效的十六进制字符\n");
            return -1;
        }
        
        // 合并高位和低位，组成一个字节
        // 高4位左移4位，与低4位进行或运算
        bytes[i] = (high << 4) | low;
    }
    
    return byte_len;
}

//将字节数组转换为十六进制字符
void bytes_to_hex(const unsigned char *bytes, int len, char *hex) {
    // 遍历每个字节，转换为2个十六进制字符
    for (int i = 0; i < len; i++) {
        // 使用sprintf将字节格式化为2位十六进制数
        // hex + i * 2 计算当前字符在输出缓冲区中的位置
        sprintf(hex + i * 2, "%02x", bytes[i]);
    }
    // 添加字符串结束符
    hex[len * 2] = '\0';
}

//功能: 读取两个十六进制字符串，进行逐字节异或，输出结果
int main() {
    // 定义输入缓冲区
    char hex1[1024], hex2[1024];           // 存储输入的十六进制字符串
    unsigned char bytes1[512], bytes2[512]; // 存储转换后的字节数据
    unsigned char result[512];              // 存储异或结果
    char hex_result[1024];                  // 存储最终的十六进制结果
    
    // 获取用户输入
    printf("请输入第一个十六进制字符串: ");
    scanf("%s", hex1);
    printf("请输入第二个十六进制字符串: ");
    scanf("%s", hex2);
    
    // 将十六进制字符串转换为字节数组
    // 将第一个字符串转换为字节
    int len1 = hex_to_bytes(hex1, bytes1, sizeof(bytes1));
    // 将第二个字符串转换为字节
    int len2 = hex_to_bytes(hex2, bytes2, sizeof(bytes2));
    
    // 检查转换是否成功
    if (len1 == -1 || len2 == -1) {
        return 1;  // 转换失败，退出程序
    }
    
    // 检查两个字符串长度是否相同
    if (len1 != len2) {
        printf("错误: 两个字符串长度必须相同\n");
        return 1;
    }
    
    // 逐字节进行异或操作
    // 遍历每个字节，对对应位置的字节进行异或运算
    for (int i = 0; i < len1; i++) {
        // 异或运算：相同为0，不同为1
        result[i] = bytes1[i] ^ bytes2[i];
    }
    
    // 将结果转换回十六进制字符串
    bytes_to_hex(result, len1, hex_result);
    
    // 输出最终结果
    printf("异或结果: %s\n", hex_result);
    
    return 0;
}
