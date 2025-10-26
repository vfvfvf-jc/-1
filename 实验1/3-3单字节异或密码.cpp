#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// 十六进制字符转数值
int hex_char_to_value(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

// 十六进制字符串转字节数组
int hex_to_bytes(const char *hex, unsigned char *bytes, int max_len) {
    int len = strlen(hex);
    // 验证十六进制字符串长度是否为偶数
    if (len % 2 != 0) {
        printf("错误: 十六进制字符串长度必须为偶数\n");
        return -1;
    }
    
    int byte_len = len / 2;
    // 检查缓冲区是否足够大
    if (byte_len > max_len) {
        printf("错误: 输入数据过长\n");
        return -1;
    }
    
    // 将每两个十六进制字符转换为一个字节
    for (int i = 0; i < byte_len; i++) {
        int high = hex_char_to_value(hex[i * 2]);      // 获取高4位
        int low = hex_char_to_value(hex[i * 2 + 1]);   // 获取低4位
        
        if (high == -1 || low == -1) {
            printf("错误: 无效的十六进制字符\n");
            return -1;
        }
        
        // 组合高4位和低4位形成一个字节
        bytes[i] = (high << 4) | low;
    }
    
    return byte_len;
}

// 计算文本的得分（基于英文字母频率）
double score_text(const unsigned char *text, int len) {
    double score = 0;
    int letter_count = 0;
    
    // 英文字母频率表（用于评估文本合理性）
    double freq[26] = {
        8.17, 1.49, 2.78, 4.25, 12.70, 2.23, 2.02, 6.09, 6.97, 0.15,
        0.77, 4.03, 2.41, 6.75, 7.51, 1.93, 0.10, 5.99, 6.33, 9.06,
        2.76, 0.98, 2.36, 0.15, 1.97, 0.07
    };
    
    for (int i = 0; i < len; i++) {
        char c = text[i];
        if (isalpha(c)) {
            letter_count++;
            // 根据字母频率加分
            if (islower(c)) {
                score += freq[c - 'a'];
            } else {
                score += freq[c - 'A'];
            }
        } else if (c == ' ') {
            score += 15.0; // 空格给予高权重
        } else if (isprint(c)) {
            score += 1.0; // 其他可打印字符给予基础分
        } else {
            score -= 10.0; // 非可打印字符惩罚
        }
    }
    
    // 如果字母比例太低，降低分数（确保是英文文本）
    if (len > 0 && (double)letter_count / len < 0.7) {
        score *= 0.5;
    }
    
    return score;
}

int main() {
    char hex_input[1024];
    unsigned char bytes[512], decrypted[512];
    
    printf("请输入十六进制编码的密文: ");
    scanf("%s", hex_input);
    
    // 将十六进制密文转换为字节数组
    int byte_len = hex_to_bytes(hex_input, bytes, sizeof(bytes));
    if (byte_len == -1) {
        return 1;
    }
    
    double best_score = -1;
    unsigned char best_key = 0;
    unsigned char best_decryption[512];
    int best_len = byte_len;
    
    // 暴力破解 - 尝试所有可能的单字节XOR密钥(0-255)
    for (int key = 0; key < 256; key++) {
        // 使用当前密钥进行XOR解密
        for (int i = 0; i < byte_len; i++) {
            decrypted[i] = bytes[i] ^ key;  // XOR运算
        }
        
        // 评估解密结果的合理性
        double current_score = score_text(decrypted, byte_len);
        
        // 保存最佳解密结果
        if (current_score > best_score) {
            best_score = current_score;
            best_key = key;
            memcpy(best_decryption, decrypted, byte_len);
        }
    }
    
    // 输出最终破解结果
    printf("\n破解结果:\n");
    printf("最可能的密钥: 0x%02x (%d) '%c'\n", 
           best_key, best_key, isprint(best_key) ? best_key : '?');
    printf("解密文本: ");
    for (int i = 0; i < best_len; i++) {
        printf("%c", isprint(best_decryption[i]) ? best_decryption[i] : '?');
    }
    printf("\n得分: %.2f\n", best_score);
    
    return 0;
}
