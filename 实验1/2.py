def find_possible_keys(subarray):  # 该函数查找将密文 subarray 解密为可见字符的所有可能密钥
    visible_characters = [chr(x) for x in range(32, 126)]  # 可见字符范围（ASCII 32~126）
    candidate_keys = list(range(0x00, 0xFF))  # 枚举所有可能的密钥值
    valid_keys = candidate_keys.copy()  # 存储潜在有效的密钥

    for key in candidate_keys:  # 对每个密钥进行测试
        for char in subarray:
            if chr(char ^ key) not in visible_characters:  # 如果解密结果不是可见字符，排除该密钥
                valid_keys.remove(key)
                break

    return valid_keys

# 给定的密文
cipher_text_hex = 'F96DE8C227A259C87EE1DA2AED57C93FE5DA36ED4EC87EF2C63AAE5B9A7EFFD673BE4ACF7BE8923C\
AB1ECE7AF2DA3DA44FCF7AE29235A24C963FF0DF3CA3599A70E5DA36BF1ECE77F8DC34BE129A6CF4D126BF\
5B9A7CFEDF3EB850D37CF0C63AA2509A76FF9227A55B9A6FE3D720A850D97AB1DD35ED5FCE6BF0D138A84C\
C931B1F121B44ECE70F6C032BD56C33FF9D320ED5CDF7AFF9226BE5BDE3FF7DD21ED56CF71F5C036A94D96\
3FF8D473A351CE3FE5DA3CB84DDB71F5C17FED51DC3FE8D732BF4D963FF3C727ED4AC87EF5DB27A451D47E\
FD9230BF47CA6BFEC12ABE4ADF72E29224A84CDF3FF5D720A459D47AF59232A35A9A7AE7D33FB85FCE7AF5\
923AA31EDB3FF7D33ABF52C33FF0D673A551D93FFCD33DA35BC831B1F43CBF1EDF67F0DF23A15B963FE5DA\
36ED68D378F4DC36BF5B9A7AFFD121B44ECE76FEDC73BE5DD27AFCD773BA5FC93FE5DA3CB859D26BB1C63C\
ED5CDF3FE2D730B84CDF3FF7DD21ED5ADF7CF0D636BE1EDB79E5D721ED57CE3FE6D320ED57D469F4DC27A8\
5A963FF3C727ED49DF3FFFDD24ED55D470E69E73AC50DE3FE5DA3ABE1EDF67F4C030A44DDF3FF5D73EA250\
C96BE3D327A84D963FE5DA32B91ED36BB1D132A31ED87AB1D021A255DF71B1C436BF479A7AF0C13AA14794'

cipher_array = [int(cipher_text_hex[i:i+2], 16) for i in range(0, len(cipher_text_hex), 2)]  # 密文的 ASCII 码数组

# 遍历密钥长度 1 到 14，测试可能的密钥
for key_length in range(1, 14):
    for position in range(key_length):
        partial_cipher = cipher_array[position::key_length]  # 按照密钥长度提取相应位置的密文部分
        possible_keys = find_possible_keys(partial_cipher)  # 找出可能的密钥值
        print(f'Key length: {key_length}, Position: {position}, Possible keys: {possible_keys}')
        if possible_keys:  # 如果有可能的密钥，尝试解密
            decrypted_chars = [chr(key ^ partial_cipher[0]) for key in possible_keys]
            print(decrypted_chars)

print('###############')
import string

def filter_possible_keys(subarray):  # 该函数根据字符类型进一步筛选密钥
    allowed_characters = string.ascii_letters + string.digits + ',. '  # 允许的字符：字母、数字、逗号、句号、空格
    candidate_keys = list(range(0x00, 0xFF))  # 枚举所有可能的密钥值
    valid_keys = candidate_keys.copy()  # 存储潜在有效的密钥

    for key in candidate_keys:  # 对每个密钥进行测试
        for char in subarray:
            if chr(char ^ key) not in allowed_characters:  # 如果解密结果不在允许字符范围，排除该密钥
                valid_keys.remove(key)
                break

    return valid_keys

# 已知密钥长度为 7，继续筛选密钥
vigenere_keys = []
for pos in range(7):
    partial_cipher = cipher_array[pos::7]
    vigenere_keys.append(filter_possible_keys(partial_cipher))

print(f'Vigenere keys: {vigenere_keys}')  # 输出可能的密钥数组

# 根据密钥解密密文
plaintext = ''.join([chr(cipher_array[i] ^ vigenere_keys[i % 7][0]) for i in range(len(cipher_array))])
print(plaintext) 
