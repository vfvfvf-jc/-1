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
    if (len % 2 != 0) {
        return -1;
    }
    
    int byte_len = len / 2;
    if (byte_len > max_len) {
        return -1;
    }
    
    for (int i = 0; i < byte_len; i++) {
        int high = hex_char_to_value(hex[i * 2]);
        int low = hex_char_to_value(hex[i * 2 + 1]);
        
        if (high == -1 || low == -1) {
            return -1;
        }
        
        bytes[i] = (high << 4) | low;
    }
    
    return byte_len;
}

// 计算文本得分
double score_text(const unsigned char *text, int len) {
    double score = 0;
    int letter_count = 0;
    
    double freq[26] = {
        8.17, 1.49, 2.78, 4.25, 12.70, 2.23, 2.02, 6.09, 6.97, 0.15,
        0.77, 4.03, 2.41, 6.75, 7.51, 1.93, 0.10, 5.99, 6.33, 9.06,
        2.76, 0.98, 2.36, 0.15, 1.97, 0.07
    };
    
    for (int i = 0; i < len; i++) {
        char c = text[i];
        if (isalpha(c)) {
            letter_count++;
            if (islower(c)) {
                score += freq[c - 'a'];
            } else {
                score += freq[c - 'A'];
            }
        } else if (c == ' ') {
            score += 15.0;
        } else if (isprint(c)) {
            score += 1.0;
        } else {
            score -= 10.0;
        }
    }
    
    if (len > 0 && (double)letter_count / len < 0.7) {
        score *= 0.5;
    }
    
    return score;
}

// 对单行密文尝试所有单字节密钥解密
int try_decrypt_single_byte_xor(const unsigned char *ciphertext, int len, 
                               unsigned char *plaintext, unsigned char *best_key, double *best_score) {
    *best_score = -1;
    
    for (int key = 0; key < 256; key++) {
        unsigned char decrypted[256];
        
        // 使用当前密钥解密
        for (int i = 0; i < len; i++) {
            decrypted[i] = ciphertext[i] ^ key;
        }
        
        // 计算得分
        double score = score_text(decrypted, len);
        
        // 更新最佳结果
        if (score > *best_score) {
            *best_score = score;
            *best_key = key;
            memcpy(plaintext, decrypted, len);
        }
    }
    
    return len;
}

int main() {
    char filename[256];
    char line[1024];
    
    printf("请输入包含多行十六进制密文的文件名: ");
    scanf("%s", filename);
    
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("无法打开文件 %s\n", filename);
        return 1;
    }
    
    double global_best_score = -1;
    int best_line_num = 0;
    unsigned char global_best_plaintext[512];
    unsigned char global_best_key;
    char global_best_ciphertext[1024];
    
    int line_num = 0;
    
    // 关键步骤1: 逐行读取密文
    while (fgets(line, sizeof(line), file)) {
        line_num++;
        
        // 移除换行符
        line[strcspn(line, "\r\n")] = 0;
        
        if (strlen(line) == 0) continue;
        
        unsigned char ciphertext_bytes[256];
        unsigned char plaintext[256];
        unsigned char best_key;
        double best_score;
        
        // 关键步骤2: 将十六进制转换为字节
        int byte_len = hex_to_bytes(line, ciphertext_bytes, sizeof(ciphertext_bytes));
        if (byte_len == -1) {
            continue; // 跳过无效行
        }
        
        // 关键步骤3: 尝试单字节异或解密
        int plaintext_len = try_decrypt_single_byte_xor(ciphertext_bytes, byte_len, 
                                                       plaintext, &best_key, &best_score);
        
        // 关键步骤4: 更新全局最佳结果
        if (best_score > global_best_score) {
            global_best_score = best_score;
            best_line_num = line_num;
            global_best_key = best_key;
            memcpy(global_best_plaintext, plaintext, plaintext_len);
            strcpy(global_best_ciphertext, line);
        }
    }
    
    fclose(file);
    
    // 输出检测结果
    if (global_best_score > 0) {
        printf("\n检测到单字符异或加密的密文:\n");
        printf("行号: %d\n", best_line_num);
        printf("密文: %s\n", global_best_ciphertext);
        printf("密钥: 0x%02x (%d) '%c'\n", 
               global_best_key, global_best_key, 
               isprint(global_best_key) ? global_best_key : '?');
        printf("明文: ");
        for (int i = 0; i < strlen((char*)global_best_plaintext); i++) {
            printf("%c", isprint(global_best_plaintext[i]) ? global_best_plaintext[i] : '?');
        }
        printf("\n得分: %.2f\n", global_best_score);
    } else {
        printf("未找到明显的单字符异或加密密文\n");
    }
    
    return 0;
}
