#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Base64编码表
const char base64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// func1：将十六进制字符转换为数值
int hex_char_to_value(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1; // 无效字符
}

// 将十六进制字符串转换为字节数组
int hex_to_bytes(const char *hex, unsigned char *bytes, int max_len) {
	int i;
    int len = strlen(hex);
    if (len % 2 != 0) {
        printf("错误: 十六进制字符串长度必须为偶数\n");
        return -1;
    }
    
    int byte_len = len / 2; //字节数是串长的二分之一（十六进制字符串转为字节数组时，两个字符转为一个字节 
    if (byte_len > max_len) {
        printf("错误: 输入数据过长\n");
        return -1;
    }
    
    for (i = 0; i < byte_len; i++) {
    	//调用func1，取字符串的连续两位的数值转换结果 
        int high = hex_char_to_value(hex[i * 2]);
        int low = hex_char_to_value(hex[i * 2 + 1]);
        
        if (high == -1 || low == -1) {
            printf("错误: 无效的十六进制字符\n");
            return -1;
        }
        
        bytes[i] = (high << 4) | low; //bytes[]是字节数组，|是连接符
									  //通过指针访问第i个元素 
    }
    
    return byte_len;
}

// Base64编码函数
void base64_encode(const unsigned char *data, int data_len, char *output) {
    int i = 0, j = 0;
    
    while (i < data_len) {
        // 每次处理3个字节，生成4个Base64字符 
        int octet_a = i < data_len ? data[i++] : 0;
        int octet_b = i < data_len ? data[i++] : 0;
        int octet_c = i < data_len ? data[i++] : 0;
        
        // 将3个字节(24位)分成4个6位组
        int triple = (octet_a << 16) | (octet_b << 8) | octet_c;
        
        output[j++] = base64_table[(triple >> 18) & 0x3F];
        output[j++] = base64_table[(triple >> 12) & 0x3F];
        output[j++] = base64_table[(triple >> 6) & 0x3F];
        output[j++] = base64_table[triple & 0x3F];
    }
    
    // 处理填充
    if (data_len % 3 == 1) {
        output[j - 2] = '=';
        output[j - 1] = '=';
    } else if (data_len % 3 == 2) {
        output[j - 1] = '=';
    }
    
    output[j] = '\0';
}

int main() {
    char hex_input[1024];
    unsigned char bytes[512];
    char base64_output[1024];
    
    printf("请输入十六进制字符串: ");
    scanf("%s", hex_input);
    
    //将十六进制转换为字节
    int byte_len = hex_to_bytes(hex_input, bytes, sizeof(bytes));//第二个参数bytes是数组名，也是数组的首地址 
    if (byte_len == -1) {
        return 1;
    }
    
    //将字节数据Base64编码
    base64_encode(bytes, byte_len, base64_output);
    
    printf("Base64编码结果: %s\n", base64_output);
    
    return 0;
}
