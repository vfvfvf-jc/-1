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
    // ��֤ʮ�������ַ��������Ƿ�Ϊż��
    if (len % 2 != 0) {
        printf("����: ʮ�������ַ������ȱ���Ϊż��\n");
        return -1;
    }
    
    int byte_len = len / 2;
    // ��黺�����Ƿ��㹻��
    if (byte_len > max_len) {
        printf("����: �������ݹ���\n");
        return -1;
    }
    
    // ��ÿ����ʮ�������ַ�ת��Ϊһ���ֽ�
    for (int i = 0; i < byte_len; i++) {
        int high = hex_char_to_value(hex[i * 2]);      // ��ȡ��4λ
        int low = hex_char_to_value(hex[i * 2 + 1]);   // ��ȡ��4λ
        
        if (high == -1 || low == -1) {
            printf("����: ��Ч��ʮ�������ַ�\n");
            return -1;
        }
        
        // ��ϸ�4λ�͵�4λ�γ�һ���ֽ�
        bytes[i] = (high << 4) | low;
    }
    
    return byte_len;
}

// �����ı��ĵ÷֣�����Ӣ����ĸƵ�ʣ�
double score_text(const unsigned char *text, int len) {
    double score = 0;
    int letter_count = 0;
    
    // Ӣ����ĸƵ�ʱ����������ı������ԣ�
    double freq[26] = {
        8.17, 1.49, 2.78, 4.25, 12.70, 2.23, 2.02, 6.09, 6.97, 0.15,
        0.77, 4.03, 2.41, 6.75, 7.51, 1.93, 0.10, 5.99, 6.33, 9.06,
        2.76, 0.98, 2.36, 0.15, 1.97, 0.07
    };
    
    for (int i = 0; i < len; i++) {
        char c = text[i];
        if (isalpha(c)) {
            letter_count++;
            // ������ĸƵ�ʼӷ�
            if (islower(c)) {
                score += freq[c - 'a'];
            } else {
                score += freq[c - 'A'];
            }
        } else if (c == ' ') {
            score += 15.0; // �ո�����Ȩ��
        } else if (isprint(c)) {
            score += 1.0; // �����ɴ�ӡ�ַ����������
        } else {
            score -= 10.0; // �ǿɴ�ӡ�ַ��ͷ�
        }
    }
    
    // �����ĸ����̫�ͣ����ͷ�����ȷ����Ӣ���ı���
    if (len > 0 && (double)letter_count / len < 0.7) {
        score *= 0.5;
    }
    
    return score;
}

int main() {
    char hex_input[1024];
    unsigned char bytes[512], decrypted[512];
    
    printf("������ʮ�����Ʊ��������: ");
    scanf("%s", hex_input);
    
    // ��ʮ����������ת��Ϊ�ֽ�����
    int byte_len = hex_to_bytes(hex_input, bytes, sizeof(bytes));
    if (byte_len == -1) {
        return 1;
    }
    
    double best_score = -1;
    unsigned char best_key = 0;
    unsigned char best_decryption[512];
    int best_len = byte_len;
    
    // �����ƽ� - �������п��ܵĵ��ֽ�XOR��Կ(0-255)
    for (int key = 0; key < 256; key++) {
        // ʹ�õ�ǰ��Կ����XOR����
        for (int i = 0; i < byte_len; i++) {
            decrypted[i] = bytes[i] ^ key;  // XOR����
        }
        
        // �������ܽ���ĺ�����
        double current_score = score_text(decrypted, byte_len);
        
        // ������ѽ��ܽ��
        if (current_score > best_score) {
            best_score = current_score;
            best_key = key;
            memcpy(best_decryption, decrypted, byte_len);
        }
    }
    
    // ��������ƽ���
    printf("\n�ƽ���:\n");
    printf("����ܵ���Կ: 0x%02x (%d) '%c'\n", 
           best_key, best_key, isprint(best_key) ? best_key : '?');
    printf("�����ı�: ");
    for (int i = 0; i < best_len; i++) {
        printf("%c", isprint(best_decryption[i]) ? best_decryption[i] : '?');
    }
    printf("\n�÷�: %.2f\n", best_score);
    
    return 0;
}
