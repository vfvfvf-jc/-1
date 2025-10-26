from typing import Callable
import base64
from Crypto.Cipher import AES

# 从文件读取 Base64 编码的密文
with open('2.txt', 'r') as file:
    encoded_ciphertext = file.read().strip()  # 读取并去掉前后空白

# 对读取的 Base64 编码内容进行解码
ciphertext = base64.b64decode(encoded_ciphertext)

# 设置密钥和初始化 AES 上下文
encryption_key = b"YELLOW SUBMARINE"
aes_ctx = AES.new(encryption_key, AES.MODE_ECB)


# CBC 解密并输出结果
def xor_bytes(byte_seq1: bytes, byte_seq2: bytes) -> bytes:
    return bytes(byte1 ^ byte2 for byte1, byte2 in zip(byte_seq1, byte_seq2))


def cbc_decrypt(encrypted_blocks: list[bytes], init_vector: bytes, cipher_function: Callable[[bytes], bytes]) -> list[
    bytes]:
    decrypted_blocks = [xor_bytes(cipher_function(encrypted_blocks[0]), init_vector)]
    for idx, encrypted_block in enumerate(encrypted_blocks[1:]):
        decrypted_blocks.append(xor_bytes(cipher_function(encrypted_block), encrypted_blocks[idx]))
    return decrypted_blocks


def split_into_blocks(message: bytes, block_size: int = 16) -> list[bytes]:
    assert len(message) % block_size == 0
    return [message[i:i + block_size] for i in range(0, len(message), block_size)]


decrypted_message = b"".join(cbc_decrypt(split_into_blocks(ciphertext), b"\x00" * 16, aes_ctx.decrypt))
print(decrypted_message)
