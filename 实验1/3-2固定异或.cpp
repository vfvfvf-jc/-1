#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//��ʮ�������ַ�ת��Ϊ��Ӧ����ֵ
int hex_char_to_value(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

//��ʮ�������ַ���ת��Ϊ�ֽ�����
int hex_to_bytes(const char *hex, unsigned char *bytes, int max_len) {
    // ��ȡ�����ַ�������
    int len = strlen(hex);
    
    // ��鳤���Ƿ�Ϊż����ÿ���ֽ���Ҫ2��ʮ�������ַ���
    if (len % 2 != 0) {
        printf("����: ʮ�������ַ������ȱ���Ϊż��\n");
        return -1;
    }
    
    int byte_len = len / 2;
    
    // �������������Ƿ��㹻
    if (byte_len > max_len) {
        printf("����: �������ݹ���\n");
        return -1;
    }
    
    // ����ÿ��ʮ�������ַ�
    for (int i = 0; i < byte_len; i++) {
        // ��ȡ��λ�͵�λ�ַ�
        int high = hex_char_to_value(hex[i * 2]);      // ��һ���ַ�����4λ��
        int low = hex_char_to_value(hex[i * 2 + 1]);   // �ڶ����ַ�����4λ��
        
        // ����ַ���Ч��
        if (high == -1 || low == -1) {
            printf("����: ��Ч��ʮ�������ַ�\n");
            return -1;
        }
        
        // �ϲ���λ�͵�λ�����һ���ֽ�
        // ��4λ����4λ�����4λ���л�����
        bytes[i] = (high << 4) | low;
    }
    
    return byte_len;
}

//���ֽ�����ת��Ϊʮ�������ַ�
void bytes_to_hex(const unsigned char *bytes, int len, char *hex) {
    // ����ÿ���ֽڣ�ת��Ϊ2��ʮ�������ַ�
    for (int i = 0; i < len; i++) {
        // ʹ��sprintf���ֽڸ�ʽ��Ϊ2λʮ��������
        // hex + i * 2 ���㵱ǰ�ַ�������������е�λ��
        sprintf(hex + i * 2, "%02x", bytes[i]);
    }
    // ����ַ���������
    hex[len * 2] = '\0';
}

//����: ��ȡ����ʮ�������ַ������������ֽ����������
int main() {
    // �������뻺����
    char hex1[1024], hex2[1024];           // �洢�����ʮ�������ַ���
    unsigned char bytes1[512], bytes2[512]; // �洢ת������ֽ�����
    unsigned char result[512];              // �洢�����
    char hex_result[1024];                  // �洢���յ�ʮ�����ƽ��
    
    // ��ȡ�û�����
    printf("�������һ��ʮ�������ַ���: ");
    scanf("%s", hex1);
    printf("������ڶ���ʮ�������ַ���: ");
    scanf("%s", hex2);
    
    // ��ʮ�������ַ���ת��Ϊ�ֽ�����
    // ����һ���ַ���ת��Ϊ�ֽ�
    int len1 = hex_to_bytes(hex1, bytes1, sizeof(bytes1));
    // ���ڶ����ַ���ת��Ϊ�ֽ�
    int len2 = hex_to_bytes(hex2, bytes2, sizeof(bytes2));
    
    // ���ת���Ƿ�ɹ�
    if (len1 == -1 || len2 == -1) {
        return 1;  // ת��ʧ�ܣ��˳�����
    }
    
    // ��������ַ��������Ƿ���ͬ
    if (len1 != len2) {
        printf("����: �����ַ������ȱ�����ͬ\n");
        return 1;
    }
    
    // ���ֽڽ���������
    // ����ÿ���ֽڣ��Զ�Ӧλ�õ��ֽڽ����������
    for (int i = 0; i < len1; i++) {
        // ������㣺��ͬΪ0����ͬΪ1
        result[i] = bytes1[i] ^ bytes2[i];
    }
    
    // �����ת����ʮ�������ַ���
    bytes_to_hex(result, len1, hex_result);
    
    // ������ս��
    printf("�����: %s\n", hex_result);
    
    return 0;
}
