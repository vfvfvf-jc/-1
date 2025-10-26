import base64
import hashlib
from Crypto.Cipher.AES import new as aes_cipher

# Base64 编码的密文
encoded_str = ("9MgYwmuPrjiecPMx61O6zIuy3MtIXQQ0E59T3xB6u0Gyf1gYs2i3K9"
               "Jxaa0zj4gTMazJuApwd6+jdyeI5iGHvhQyDHGVlAuYTgJrbFDrfB22Fpil2NfNnWFBTXyf7SDI")
ciphertext_bytes = base64.b64decode(encoded_str)

# 校验和函数
def calculate_checksum(mrz_segment):
    weights = [7, 3, 1]

    def get_mapped_values():
        for index, char in enumerate(mrz_segment):
            ascii_value = ord(char)
            if '0' <= char <= '9':  # 数字字符处理
                yield index, ascii_value - ord('0')
            elif 'A' <= char <= 'Z':  # 大写字母处理
                yield index, ascii_value - ord('A') + 10

    return sum(value * weights[i % len(weights)] for i, value in get_mapped_values()) % 10

# 校验示例
assert calculate_checksum("111116") == 7

# MRZ 示例数据
mrz_code = "12345678<8<<<1110182<1111167<<<<<<<<<<<<<<<4"

# 生成密钥种子
key_seed = hashlib.sha1((mrz_code[:10] + mrz_code[13:20] + mrz_code[21:28]).encode()).digest()[:16]

# 生成密钥
def generate_key(seed, counter):
    return bytes(
        ((byte & 0b11111110) | ((byte & 0b11111110).bit_count() & 1) ^ 1)
        for byte in hashlib.sha1(seed + bytes([0, 0, 0, counter])).digest()[:16]
    )

# 密钥生成并进行 AES 解密
encryption_key = generate_key(key_seed, 1)
aes_ctx = aes_cipher(encryption_key, 2, b"\x00" * 16)
decrypted_data = aes_ctx.decrypt(ciphertext_bytes)

# 去除填充 - 去除所有尾随的零字节和可能的填充标记
clean_data = decrypted_data.rstrip(b'\x00').rstrip(b'\x01')

print("原始数据（带填充）:", decrypted_data)
print("去除填充后:", clean_data)
print("文本内容:", clean_data.decode('utf-8'))
