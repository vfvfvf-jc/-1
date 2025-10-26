import base64
import os
from Crypto.Cipher import AES
from Crypto.Util import Padding
import string


# 加密 Oracle，使用随机密钥和 ECB 模式加密
def encryption_oracle(oracle: bytes) -> bytes:
    key = os.urandom(16)  # 生成随机 16 字节密钥
    secret_suffix = base64.b64decode("""
Um9sbGluJyBpbiBteSA1LjAKV2l0aCBteSByYWctdG9wIGRvd24gc28gbXkg
aGFpciBjYW4gYmxvdwpUaGUgZ2lybGllcyBvbiBzdGFuZGJ5IHdhdmluZyBq
dXN0IHRvIHNheSBoaQpEaWQgeW91IHN0b3A/IE5vLCBJIGp1c3QgZHJvdmUg
YnkK""")  # 固定的 Base64 编码密文
    plaintext = oracle + secret_suffix
    padded_plaintext = Padding.pad(plaintext, 16)  # 对明文进行填充
    cipher = AES.new(key, AES.MODE_ECB)  # 使用 ECB 模式创建加密器
    return cipher.encrypt(padded_plaintext)


# 计算初始未知字符串的长度
initial_unknown_len = len(encryption_oracle(b""))
unknown_len = initial_unknown_len
assert unknown_len % 16 == 0  # 确保长度是 16 字节的倍数

# 通过输入长度的变化，确定未知明文的长度
for i in range(16):
    if len(encryption_oracle(b"A" * i)) != initial_unknown_len:
        unknown_len = initial_unknown_len - i
        break

# 可打印的字符集，用于尝试破解
printable_chars = string.printable.encode()


# 破解未知的明文
def break_ecb_cipher(known_plaintext: bytes) -> bool:
    while True:
        partial_known = known_plaintext[-15:]  # 获取已知明文的最后 15 字节
        partial_known = b"\x00" * (15 - len(partial_known)) + partial_known  # 不足 15 字节则补零

        matched_bytes = []
        for char in printable_chars:
            # 构造 oracle 输入，使未知明文的第一个字节对齐到块的最后
            oracle_input = partial_known + bytes([char]) + b"\x00" * (15 - len(known_plaintext) % 16)
            encrypted_data = encryption_oracle(oracle_input)

            # 比较密文中的块，寻找成功碰撞的字符
            if encrypted_data[15] == encrypted_data[(len(known_plaintext) // 16) * 16 + 31]:
                matched_bytes.append(char)

        if len(matched_bytes) == 1:  # 找到唯一的匹配字符
            known_plaintext += bytes(matched_bytes)
            print(known_plaintext)

            if len(known_plaintext) == unknown_len:  # 明文长度已达到未知部分的长度，破解完成
                return True
        elif len(matched_bytes) == 0:  # 无匹配字符，破解失败
            return False
        else:  # 多个匹配字符时，递归尝试每个字符
            for matched_char in matched_bytes:
                if break_ecb_cipher(known_plaintext + bytes([matched_char])):  # 递归搜索
                    return True


# 开始破解未知明文
break_ecb_cipher(b'')
