/*#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

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

// 计算两个等长字符串的汉明距离
int hamming_distance(const unsigned char *str1, const unsigned char *str2, int len) {
    int distance = 0;
    for (int i = 0; i < len; i++) {
        unsigned char xor_result = str1[i] ^ str2[i];  // 将变量名从 xor 改为 xor_result
        // 计算xor_result中1的个数（汉明重量）
        while (xor_result) {
            distance += xor_result & 1;
            xor_result >>= 1;
        }
    }
    return distance;
}

// 计算可能的密钥长度
int find_key_length(const unsigned char *ciphertext, int ciphertext_len, int max_key_len) {
    double best_normalized_distance = 1000.0;
    int best_key_len = 0;
    
    // 尝试不同的密钥长度
    for (int key_len = 2; key_len <= max_key_len && key_len * 8 <= ciphertext_len; key_len++) {
        double total_distance = 0;
        int blocks_compared = 0;
        
        // 比较多个块对
        for (int i = 0; i < 4; i++) {
            int start1 = i * key_len;
            int start2 = (i + 1) * key_len;
            
            if (start2 + key_len <= ciphertext_len) {
                int dist = hamming_distance(ciphertext + start1, 
                                          ciphertext + start2, key_len);
                total_distance += (double)dist / key_len; // 标准化距离
                blocks_compared++;
            }
        }
        
        if (blocks_compared > 0) {
            double normalized_distance = total_distance / blocks_compared;
            
            if (normalized_distance < best_normalized_distance) {
                best_normalized_distance = normalized_distance;
                best_key_len = key_len;
            }
        }
    }
    
    return best_key_len;
}

// 计算文本得分（基于英文字母频率）
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

// 对单字节异或进行暴力破解
unsigned char break_single_byte_xor(const unsigned char *ciphertext, int len, double *best_score) {
    unsigned char best_key = 0;
    *best_score = -1;
    
    for (int key = 0; key < 256; key++) {
        unsigned char decrypted[4096];
        
        for (int i = 0; i < len; i++) {
            decrypted[i] = ciphertext[i] ^ key;
        }
        
        double score = score_text(decrypted, len);
        
        if (score > *best_score) {
            *best_score = score;
            best_key = key;
        }
    }
    
    return best_key;
}

int main() {
    char hex_input[8192];
    unsigned char ciphertext[4096];
    
    printf("请输入十六进制编码的重复密钥异或密文: ");
    scanf("%s", hex_input);
    
    // 关键步骤1: 将十六进制密文转换为字节
    int ciphertext_len = hex_to_bytes(hex_input, ciphertext, sizeof(ciphertext));
    if (ciphertext_len == -1) {
        printf("错误: 无效的十六进制字符串\n");
        return 1;
    }
    
    printf("密文长度: %d 字节\n", ciphertext_len);
    
    // 关键步骤2: 寻找可能的密钥长度
    int max_key_len = 40;
    if (max_key_len > ciphertext_len / 2) {
        max_key_len = ciphertext_len / 2;
    }
    
    int key_length = find_key_length(ciphertext, ciphertext_len, max_key_len);
    printf("推测的密钥长度: %d\n", key_length);
    
    // 关键步骤3: 将密文按密钥长度分块，每块使用单字节异或破解
    unsigned char key[key_length];
    unsigned char plaintext[4096];
    
    for (int key_pos = 0; key_pos < key_length; key_pos++) {
        // 提取该密钥位置对应的所有字节
        int block_len = 0;
        unsigned char block[1024];
        
        for (int i = key_pos; i < ciphertext_len; i += key_length) {
            block[block_len++] = ciphertext[i];
        }
        
        // 破解单字节异或
        double score;
        key[key_pos] = break_single_byte_xor(block, block_len, &score);
        
        printf("密钥位置 %d: 0x%02x ('%c'), 得分: %.2f\n", 
               key_pos, key[key_pos], 
               isprint(key[key_pos]) ? key[key_pos] : '?', score);
    }
    
    // 关键步骤4: 使用找到的密钥解密整个密文
    for (int i = 0; i < ciphertext_len; i++) {
        plaintext[i] = ciphertext[i] ^ key[i % key_length];
    }
    
    // 输出结果
    printf("\n破解结果:\n");
    printf("密钥: ");
    for (int i = 0; i < key_length; i++) {
        printf("%c", isprint(key[i]) ? key[i] : '?');
    }
    printf(" (十六进制: ");
    for (int i = 0; i < key_length; i++) {
        printf("%02x", key[i]);
    }
    printf(")\n");
    
    printf("解密后的明文:\n");
    for (int i = 0; i < ciphertext_len; i++) {
        printf("%c", isprint(plaintext[i]) ? plaintext[i] : '?');
    }
    printf("\n");
    
    return 0;
}*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <ctype.h>

// Base64 解码函数
unsigned char* base64_decode(const char* input, size_t* out_len) {
    const char base64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    size_t len = strlen(input);
    if (len == 0) {
        *out_len = 0;
        return NULL;
    }
    if (len % 4 != 0) return NULL;

    *out_len = (len / 4) * 3;
    if (input[len-1] == '=') (*out_len)--;
    if (input[len-2] == '=') (*out_len)--;

    unsigned char* decoded = (unsigned char*)malloc(*out_len + 1); // 添加类型转换
    if (!decoded) return NULL;

    size_t i, j = 0;
    unsigned char a, b, c, d;
    for (i = 0; i < len; i += 4) {
        const char* pos_a = strchr(base64_table, input[i]);
        const char* pos_b = strchr(base64_table, input[i+1]);
        const char* pos_c = strchr(base64_table, input[i+2]);
        const char* pos_d = strchr(base64_table, input[i+3]);
        
        if (!pos_a || !pos_b) break;
        a = (unsigned char)(pos_a - base64_table);
        b = (unsigned char)(pos_b - base64_table);

        decoded[j++] = (a << 2) | (b >> 4);
        
        if (input[i+2] != '=' && pos_c) {
            c = (unsigned char)(pos_c - base64_table);
            decoded[j++] = (b << 4) | (c >> 2);
        }
        if (input[i+3] != '=' && pos_d) {
            d = (unsigned char)(pos_d - base64_table);
            decoded[j++] = (c << 6) | d;
        }
    }
    *out_len = j;
    decoded[j] = '\0';
    return decoded;
}

// 计算两个等长字符串的汉明距离
int hamming_distance(const unsigned char* s1, const unsigned char* s2, int len) {
    int dist = 0;
    for (int i = 0; i < len; i++) {
        unsigned char x = s1[i] ^ s2[i];
        while (x) {
            dist += x & 1;
            x >>= 1;
        }
    }
    return dist;
}

// 猜测密钥长度
int guess_keysize(const unsigned char* data, int datalen) {
    int best_keysize = -1;
    float best_score = 1e9;

    for (int keysize = 2; keysize <= 40; keysize++) {
        if (keysize * 8 > datalen) break;

        float total = 0;
        int blocks = 4;
        int count = 0;
        for (int i = 0; i < blocks; i++) {
            for (int j = i+1; j < blocks; j++) {
                if ((j+1)*keysize <= datalen) {
                    total += hamming_distance(data + i*keysize, data + j*keysize, keysize);
                    count++;
                }
            }
        }
        if (count == 0) continue;
        float avg = total / count;
        float normalized = avg / keysize;

        if (normalized < best_score) {
            best_score = normalized;
            best_keysize = keysize;
        }
    }
    return best_keysize;
}

// 英文字母频率表
float freq_table[256] = {0};
void init_freq_table() {
    freq_table[' '] = 0.15;
    freq_table['e'] = 0.127;
    freq_table['t'] = 0.091;
    freq_table['a'] = 0.082;
    freq_table['o'] = 0.075;
    freq_table['i'] = 0.070;
    freq_table['n'] = 0.067;
    freq_table['s'] = 0.063;
    freq_table['h'] = 0.061;
    freq_table['r'] = 0.060;
    freq_table['d'] = 0.043;
    freq_table['l'] = 0.040;
    freq_table['c'] = 0.028;
    freq_table['u'] = 0.028;
    freq_table['m'] = 0.024;
    freq_table['w'] = 0.024;
    freq_table['f'] = 0.022;
    freq_table['g'] = 0.020;
    freq_table['y'] = 0.020;
    freq_table['p'] = 0.019;
    freq_table['b'] = 0.015;
    freq_table['v'] = 0.0098;
    freq_table['k'] = 0.0077;
    freq_table['j'] = 0.0015;
    freq_table['x'] = 0.0015;
    freq_table['q'] = 0.00095;
    freq_table['z'] = 0.00074;
}

// 对单字节 XOR 进行频率分析，返回最佳密钥字节
unsigned char break_single_xor(const unsigned char* data, int len) {
    float best_score = -1e9;
    unsigned char best_key = 0;

    for (int k = 0; k < 256; k++) {
        float score = 0;
        for (int i = 0; i < len; i++) {
            unsigned char dec = data[i] ^ k;
            if (dec >= 'A' && dec <= 'Z') dec = dec - 'A' + 'a';
            score += freq_table[dec];
        }
        if (score > best_score) {
            best_score = score;
            best_key = k;
        }
    }
    return best_key;
}

// 重复密钥 XOR 解密
unsigned char* repeating_key_xor(const unsigned char* data, int datalen, 
                                 const unsigned char* key, int keylen, 
                                 int* out_len) {
    unsigned char* decrypted = (unsigned char*)malloc(datalen + 1); // 添加类型转换
    if (!decrypted) return NULL;
    
    for (int i = 0; i < datalen; i++) {
        decrypted[i] = data[i] ^ key[i % keylen];
    }
    decrypted[datalen] = '\0';
    *out_len = datalen;
    return decrypted;
}

int main() {
    // 初始化频率表
    init_freq_table();
    
    // 直接使用题目中的 Base64 密文
    const char* b64_str = 
        "HUIfTQsPAh9PE048GmllH0kcDk4TAQsHThsBFkU2AB4BSWQgVB0dQzNTTmVS"
        "BgBHVBwNRU0HBAxTEjwMHghJGgkRTxRMIRpHKwAFHUdZEQQJAGQmB1MANxYG"
        "DBoXQR0BUlQwXwAgEwoFR08SSAhFTmU+Fgk4RQYFCBpGB08fWXh+amI2DB0P"
        "QQ1IBlUaGwAdQnQEHgFJGgkRAlJ6f0kASDoAGhNJGk9FSA8dDVMEOgFSGQEL"
        "QRMGAEwxX1NiFQYHCQdUCxdBFBZJeTM1CxsBBQ9GB08dTnhOSCdSBAcMRVhI"
        "CEEATyBUCHQLHRlJAgAOFlwAUjBpZR9JAgJUAAELB04CEFMBJhAVTQIHAh9P"
        "G054MGk2UgoBCVQGBwlTTgIQUwg7EAYFSQ8PEE87ADpfRyscSWQzT1QCEFMa"
        "TwUWEXQMBk0PAg4DQ1JMPU4ALwtJDQhOFw0VVB1PDhxFXigLTRkBEgcKVVN4"
        "Tk9iBgELR1MdDAAAFwoFHww6Ql5NLgFBIg4cSTRWQWI1Bk9HKn47CE8BGwFT"
        "QjcEBx4MThUcDgYHKxpUKhdJGQZZVCFFVwcDBVMHMUV4LAcKQR0JUlk3TwAm"
        "HQdJEwATARNFTg5JFwQ5C15NHQYEGk94dzBDADsdHE4UVBUaDE5JTwgHRTkA"
        "Umc6AUETCgYAN1xGYlUKDxJTEUgsAA0ABwcXOwlSGQELQQcbE0c9GioWGgwc"
        "AgcHSAtPTgsAABY9C1VNCAINGxgXRHgwaWUfSQcJABkRRU8ZAUkDDTUWF01j"
        "OgkRTxVJKlZJJwFJHQYADUgRSAsWSR8KIgBSAAxOABoLUlQwW1RiGxpOCEtU"
        "YiROCk8gUwY1C1IJCAACEU8QRSxORTBSHQYGTlQJC1lOBAAXRTpCUh0FDxhU"
        "ZXhzLFtHJ1JbTkoNVDEAQU4bARZFOwsXTRAPRlQYE042WwAuGxoaAk5UHAoA"
        "ZCYdVBZ0ChQLSQMYVAcXQTwaUy1SBQsTAAAAAAAMCggHRSQJExRJGgkGAAdH"
        "MBoqER1JJ0dDFQZFRhsBAlMMIEUHHUkPDxBPH0EzXwArBkkdCFUaDEVHAQAN"
        "U29lSEBAWk44G09fDXhxTi0RAk4ITlQbCk0LTx4cCjBFeCsGHEETAB1EeFZV"
        "IRlFTi4AGAEORU4CEFMXPBwfCBpOAAAdHUMxVVUxUmM9ElARGgZBAg4PAQQz"
        "DB4EGhoIFwoKUDFbTCsWBg0OTwEbRSonSARTBDpFFwsPCwIATxNOPBpUKhMd"
        "Th5PAUgGQQBPCxYRdG87TQoPD1QbE0s9GkFiFAUXR0cdGgkADwENUwg1DhdN"
        "AQsTVBgXVHYaKkg7TgNHTB0DAAA9DgQACjpFX0BJPQAZHB1OeE5PYjYMAg5M"
        "FQBFKjoHDAEAcxZSAwZOBREBC0k2HQxiKwYbR0MVBkVUHBZJBwp0DRMDDk5r"
        "NhoGACFVVWUeBU4MRREYRVQcFgAdQnQRHU0OCxVUAgsAK05ZLhdJZChWERpF"
        "QQALSRwTMRdeTRkcABcbG0M9Gk0jGQwdR1ARGgNFDRtJeSchEVIDBhpBHQlS"
        "WTdPBzAXSQ9HTBsJA0UcQUl5bw0KB0oFAkETCgYANlVXKhcbC0sAGgdFUAIO"
        "ChZJdAsdTR0HDBFDUk43GkcrAAUdRyonBwpOTkJEUyo8RR8USSkOEENSSDdX"
        "RSAdDRdLAA0HEAAeHQYRBDYJC00MDxVUZSFQOV1IJwYdB0dXHRwNAA9PGgMK"
        "OwtTTSoBDBFPHU54W04mUhoPHgAdHEQAZGU/OjV6RSQMBwcNGA5SaTtfADsX"
        "GUJHWREYSQAnSARTBjsIGwNOTgkVHRYANFNLJ1IIThVIHQYKAGQmBwcKLAwR"
        "DB0HDxNPAU94Q083UhoaBkcTDRcAAgYCFkU1RQUEBwFBfjwdAChPTikBSR0T"
        "TwRIEVIXBgcURTULFk0OBxMYTwFUN0oAIQAQBwkHVGIzQQAGBR8EdCwRCEkH"
        "ElQcF0w0U05lUggAAwANBxAAHgoGAwkxRRMfDE4DARYbTn8aKmUxCBsURVQf"
        "DVlOGwEWRTIXFwwCHUEVHRcAMlVDKRsHSUdMHQMAAC0dCAkcdCIeGAxOazkA"
        "BEk2HQAjHA1OAFIbBxNJAEhJBxctDBwKSRoOVBwbTj8aQS4dBwlHKjUECQAa"
        "BxscEDMNUhkBC0ETBxdULFUAJQAGARFJGk9FVAYGGlMNMRcXTRoBDxNPeG43"
        "TQA7HRxJFUVUCQhBFAoNUwctRQYFDE43PT9SUDdJUydcSWRtcwANFVAHAU5T"
        "FjtFGgwbCkEYBhlFeFsABRcbAwZOVCYEWgdPYyARNRcGAQwKQRYWUlQwXwAg"
        "ExoLFAAcARFUBwFOUwImCgcDDU5rIAcXUj0dU2IcBk4TUh0YFUkASEkcC3QI"
        "GwMMQkE9SB8AMk9TNlIOCxNUHQZCAAoAHh1FXjYCDBsFABkOBkk7FgALVQRO"
        "D0EaDwxOSU8dGgI8EVIBAAUEVA5SRjlUQTYbCk5teRsdRVQcDhkDADBFHwhJ"
        "AQ8XClJBNl4AC1IdBghVEwARABoHCAdFXjwdGEkDCBMHBgAwW1YnUgAaRyon"
        "B0VTGgoZUwE7EhxNCAAFVAMXTjwaTSdSEAESUlQNBFJOZU5LXHQMHE0EF0EA"
        "Bh9FeRp5LQdFTkAZREgMU04CEFMcMQQAQ0lkay0ABwcqXwA1FwgFAk4dBkIA"
        "CA4aB0l0PD1MSQ8PEE87ADtbTmIGDAILAB0cRSo3ABwBRTYKFhROHUETCgZU"
        "MVQHYhoGGksABwdJAB0ASTpFNwQcTRoDBBgDUkksGioRHUkKCE5THEVCC08E"
        "EgF0BBwJSQoOGkgGADpfADETDU5tBzcJEFMLTx0bAHQJCx8ADRJUDRdMN1RH"
        "YgYGTi5jMURFeQEaSRAEOkURDAUCQRkKUmQ5XgBIKwYbQFIRSBVJGgwBGgtz"
        "RRNNDwcVWE8BT3hJVCcCSQwGQx9IBE4KTwwdASEXF01jIgQATwZIPRpXKwYK"
        "BkdEGwsRTxxDSToGMUlSCQZOFRwKUkQ5VEMnUh0BR0MBGgAAZDwGUwY7CBdN"
        "HB5BFwMdUz0aQSwWSQoITlMcRUILTxoCEDUXF01jNw4BTwVBNlRBYhAIGhNM"
        "EUgIRU5CRFMkOhwGBAQLTVQOHFkvUkUwF0lkbXkbHUVUBgAcFA0gRQYFCBpB"
        "PU8FQSsaVycTAkJHYhsRSQAXABxUFzFFFggICkEDHR1OPxoqER1JDQhNEUgK"
        "TkJPDAUAJhwQAg0XQRUBFgArU04lUh0GDlNUGwpOCU9jeTY1HFJARE4xGA4L"
        "ACxSQTZSDxsJSw1ICFUdBgpTNjUcXk0OAUEDBxtUPRpCLQtFTgBPVB8NSRoK"
        "SREKLUUVAklkERgOCwAsUkE2Ug8bCUsNSAhVHQYKUyI7RQUFABoEVA0dWXQa"
        "Ry1SHgYOVBFIB08XQ0kUCnRvPgwQTgUbGBwAOVREYhAGAQBJEUgETgpPGR8E"
        "LUUGBQgaQRIaHEshGk03AQANR1QdBAkAFwAcUwE9AFxNY2QxGA4LACxSQTZS"
        "DxsJSw1ICFUdBgpTJjsIF00GAE1ULB1NPRpPLF5JAgJUVAUAAAYKCAFFXjUe"
        "DBBOFRwOBgA+T04pC0kDElMdC0VXBgYdFkU2CgtNEAEUVBwTWXhTVG5SGg8e"
        "AB0cRSo+AwgKRSANExlJCBQaBAsANU9TKxFJL0dMHRwRTAtPBRwQMAAATQcB"
        "FlRlIkw5QwA2GggaR0YBBg5ZTgIcAAw3SVIaAQcVEU8QTyEaYy0fDE4ITlhI"
        "Jk8DCkkcC3hFMQIEC0EbAVIqCFZBO1IdBgZUVA4QTgUWSR4QJwwRTWM=";
    
    printf("正在解密...\n");
    
    // Base64 解码
    size_t cipher_len;
    unsigned char* ciphertext = base64_decode(b64_str, &cipher_len);
    if (!ciphertext) {
        printf("Base64 解码失败\n");
        return 1;
    }
    
    printf("密文长度: %zu 字节\n", cipher_len);
    
    // 猜测密钥长度
    int keysize = guess_keysize(ciphertext, cipher_len);
    printf("猜测的密钥长度: %d\n", keysize);
    
    if (keysize <= 0 || keysize > cipher_len) {
        printf("密钥长度猜测失败\n");
        free(ciphertext);
        return 1;
    }
    
    // 破解每个密钥字节
    unsigned char* key = (unsigned char*)malloc(keysize + 1); // 添加类型转换
    if (!key) {
        free(ciphertext);
        return 1;
    }
    
    for (int k = 0; k < keysize; k++) {
        // 提取每个密钥字节对应的密文字节
        int block_len = (cipher_len - k + keysize - 1) / keysize;
        unsigned char* block = (unsigned char*)malloc(block_len); // 添加类型转换
        if (!block) {
            free(ciphertext);
            free(key);
            return 1;
        }
        
        for (int i = 0; i < block_len; i++) {
            int idx = k + i * keysize;
            if (idx < cipher_len) {
                block[i] = ciphertext[idx];
            }
        }
        
        // 破解单字节 XOR
        key[k] = break_single_xor(block, block_len);
        free(block);
    }
    key[keysize] = '\0';
    
    printf("破解的密钥: ");
    for (int i = 0; i < keysize; i++) {
        if (isprint(key[i])) {
            printf("%c", key[i]);
        } else {
            printf("\\x%02x", key[i]);
        }
    }
    printf("\n");
    
    // 使用密钥解密
    int plain_len;
    unsigned char* plaintext = repeating_key_xor(ciphertext, cipher_len, key, keysize, &plain_len);
    
    printf("\n解密后的明文:\n");
    printf("========================================\n");
    printf("%s\n", plaintext);
    printf("========================================\n");
    
    // 清理内存
    free(ciphertext);
    free(key);
    free(plaintext);
    
    printf("\n解密完成！\n");
    
    return 0;
}
