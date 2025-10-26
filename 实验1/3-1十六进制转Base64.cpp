#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Base64�����
const char base64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// func1����ʮ�������ַ�ת��Ϊ��ֵ
int hex_char_to_value(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1; // ��Ч�ַ�
}

// ��ʮ�������ַ���ת��Ϊ�ֽ�����
int hex_to_bytes(const char *hex, unsigned char *bytes, int max_len) {
	int i;
    int len = strlen(hex);
    if (len % 2 != 0) {
        printf("����: ʮ�������ַ������ȱ���Ϊż��\n");
        return -1;
    }
    
    int byte_len = len / 2; //�ֽ����Ǵ����Ķ���֮һ��ʮ�������ַ���תΪ�ֽ�����ʱ�������ַ�תΪһ���ֽ� 
    if (byte_len > max_len) {
        printf("����: �������ݹ���\n");
        return -1;
    }
    
    for (i = 0; i < byte_len; i++) {
    	//����func1��ȡ�ַ�����������λ����ֵת����� 
        int high = hex_char_to_value(hex[i * 2]);
        int low = hex_char_to_value(hex[i * 2 + 1]);
        
        if (high == -1 || low == -1) {
            printf("����: ��Ч��ʮ�������ַ�\n");
            return -1;
        }
        
        bytes[i] = (high << 4) | low; //bytes[]���ֽ����飬|�����ӷ�
									  //ͨ��ָ����ʵ�i��Ԫ�� 
    }
    
    return byte_len;
}

// Base64���뺯��
void base64_encode(const unsigned char *data, int data_len, char *output) {
    int i = 0, j = 0;
    
    while (i < data_len) {
        // ÿ�δ���3���ֽڣ�����4��Base64�ַ� 
        int octet_a = i < data_len ? data[i++] : 0;
        int octet_b = i < data_len ? data[i++] : 0;
        int octet_c = i < data_len ? data[i++] : 0;
        
        // ��3���ֽ�(24λ)�ֳ�4��6λ��
        int triple = (octet_a << 16) | (octet_b << 8) | octet_c;
        
        output[j++] = base64_table[(triple >> 18) & 0x3F];
        output[j++] = base64_table[(triple >> 12) & 0x3F];
        output[j++] = base64_table[(triple >> 6) & 0x3F];
        output[j++] = base64_table[triple & 0x3F];
    }
    
    // �������
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
    
    printf("������ʮ�������ַ���: ");
    scanf("%s", hex_input);
    
    //��ʮ������ת��Ϊ�ֽ�
    int byte_len = hex_to_bytes(hex_input, bytes, sizeof(bytes));//�ڶ�������bytes����������Ҳ��������׵�ַ 
    if (byte_len == -1) {
        return 1;
    }
    
    //���ֽ�����Base64����
    base64_encode(bytes, byte_len, base64_output);
    
    printf("Base64������: %s\n", base64_output);
    
    return 0;
}
