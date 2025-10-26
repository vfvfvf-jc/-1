#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// ʮ�������ַ�ת��ֵ
int hex_char_to_value(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

// ʮ�������ַ���ת�ֽ�����
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

// �����ı��÷�
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

// �Ե������ĳ������е��ֽ���Կ����
int try_decrypt_single_byte_xor(const unsigned char *ciphertext, int len, 
                               unsigned char *plaintext, unsigned char *best_key, double *best_score) {
    *best_score = -1;
    
    for (int key = 0; key < 256; key++) {
        unsigned char decrypted[256];
        
        // ʹ�õ�ǰ��Կ����
        for (int i = 0; i < len; i++) {
            decrypted[i] = ciphertext[i] ^ key;
        }
        
        // ����÷�
        double score = score_text(decrypted, len);
        
        // ������ѽ��
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
    
    printf("�������������ʮ���������ĵ��ļ���: ");
    scanf("%s", filename);
    
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("�޷����ļ� %s\n", filename);
        return 1;
    }
    
    double global_best_score = -1;
    int best_line_num = 0;
    unsigned char global_best_plaintext[512];
    unsigned char global_best_key;
    char global_best_ciphertext[1024];
    
    int line_num = 0;
    
    // �ؼ�����1: ���ж�ȡ����
    while (fgets(line, sizeof(line), file)) {
        line_num++;
        
        // �Ƴ����з�
        line[strcspn(line, "\r\n")] = 0;
        
        if (strlen(line) == 0) continue;
        
        unsigned char ciphertext_bytes[256];
        unsigned char plaintext[256];
        unsigned char best_key;
        double best_score;
        
        // �ؼ�����2: ��ʮ������ת��Ϊ�ֽ�
        int byte_len = hex_to_bytes(line, ciphertext_bytes, sizeof(ciphertext_bytes));
        if (byte_len == -1) {
            continue; // ������Ч��
        }
        
        // �ؼ�����3: ���Ե��ֽ�������
        int plaintext_len = try_decrypt_single_byte_xor(ciphertext_bytes, byte_len, 
                                                       plaintext, &best_key, &best_score);
        
        // �ؼ�����4: ����ȫ����ѽ��
        if (best_score > global_best_score) {
            global_best_score = best_score;
            best_line_num = line_num;
            global_best_key = best_key;
            memcpy(global_best_plaintext, plaintext, plaintext_len);
            strcpy(global_best_ciphertext, line);
        }
    }
    
    fclose(file);
    
    // ��������
    if (global_best_score > 0) {
        printf("\n��⵽���ַ������ܵ�����:\n");
        printf("�к�: %d\n", best_line_num);
        printf("����: %s\n", global_best_ciphertext);
        printf("��Կ: 0x%02x (%d) '%c'\n", 
               global_best_key, global_best_key, 
               isprint(global_best_key) ? global_best_key : '?');
        printf("����: ");
        for (int i = 0; i < strlen((char*)global_best_plaintext); i++) {
            printf("%c", isprint(global_best_plaintext[i]) ? global_best_plaintext[i] : '?');
        }
        printf("\n�÷�: %.2f\n", global_best_score);
    } else {
        printf("δ�ҵ����Եĵ��ַ�����������\n");
    }
    
    return 0;
}
