import os
import random
from Crypto.Cipher import AES
from Crypto.Util import Padding


# 生成随机密钥
def generate_random_key() -> bytes:
    return os.urandom(16)


# 生成随机填充数据
def generate_random_padding() -> bytes:
    return os.urandom(random.randint(5, 10))


# 加密 Oracle：随机选择 ECB 或 CBC 模式进行加密
def encryption_oracle(key: bytes, message: bytes) -> tuple[bytes, int]:
    encryption_mode = random.choice([AES.MODE_ECB, AES.MODE_CBC])  # 随机选择加密模式
    padded_message = generate_random_padding() + message + generate_random_padding()  # 加入随机填充
    padded_message = Padding.pad(padded_message, 16)  # 对消息进行填充以匹配块大小

    if encryption_mode == AES.MODE_ECB:
        cipher = AES.new(key, AES.MODE_ECB)
        return cipher.encrypt(padded_message), AES.MODE_ECB
    elif encryption_mode == AES.MODE_CBC:
        iv = generate_random_key()  # 生成随机初始化向量
        cipher = AES.new(key, AES.MODE_CBC, iv)
        return cipher.encrypt(padded_message), AES.MODE_CBC

    assert False, "This line should never be reached"  # 防止意外情况


# 检测加密模式，基于密文的重复块判断是否为 ECB 模式
def detect_encryption_mode(ciphertext: bytes) -> int:
    block_size = 16
    # 将密文按 16 字节块进行分割
    ciphertext_blocks = [ciphertext[i: i + block_size] for i in range(0, len(ciphertext), block_size)]
    # 如果存在重复块，则判定为 ECB 模式
    if len(ciphertext_blocks) != len(set(ciphertext_blocks)):
        return AES.MODE_ECB
    return AES.MODE_CBC


# 设置密钥和要加密的消息
key = generate_random_key()
test_message = b"\x00" * 16 * 3  # 创建一个长度为 48 字节的测试消息

# 使用 Oracle 进行 100 次加密
encrypted_data = [encryption_oracle(key, test_message) for _ in range(100)]

# 检测模式并计算检测的正确率
correct_detections = sum(detect_encryption_mode(ciphertext) == mode for ciphertext, mode in encrypted_data)
print(f"Detection Accuracy: {correct_detections / len(encrypted_data):.2%}")
