from Crypto.Cipher import AES
from Crypto.Util.Padding import pad, unpad
import os

def working_cbc_bit_flip_attack():
    """
    真正能工作的CBC位翻转攻击
    """
    key = os.urandom(16)
    iv = os.urandom(16)
    
    def encrypt(data):
        cipher = AES.new(key, AES.MODE_CBC, iv)
        return cipher.encrypt(pad(data, AES.block_size))
    
    def decrypt_no_check(ciphertext):
        cipher = AES.new(key, AES.MODE_CBC, iv)
        return cipher.decrypt(ciphertext)
    
    # 关键：确保目标字符串完全在一个块内
    # 使用精确控制的数据结构
    prefix = b"comment=test;"
    target = b"admin=false"  # 11字节
    suffix = b";extra=data"
    
    # 计算填充，确保目标字符串在完整的块中
    block_size = 16
    prefix_len = len(prefix)
    
    # 调整前缀长度，使目标字符串从新块的开始位置开始
    padding_needed = block_size - (prefix_len % block_size)
    adjusted_prefix = prefix + b"X" * padding_needed
    
    original_plaintext = adjusted_prefix + target + suffix
    print(f"构造的明文: {original_plaintext}")
    
    # 显示块结构
    blocks = [original_plaintext[i:i+block_size] for i in range(0, len(original_plaintext), block_size)]
    print("\n块结构分析:")
    for i, block in enumerate(blocks):
        print(f"块{i}: {block} (长度: {len(block)})")
    
    # 加密
    ciphertext = encrypt(original_plaintext)
    
    # 执行位翻转攻击
    modified_ciphertext = bytearray(ciphertext)
    
    # 目标字符串在哪个块？
    target_block_index = len(adjusted_prefix) // block_size
    print(f"\n目标字符串在块 {target_block_index}")
    
    # 修改前一个块（target_block_index-1）来影响目标块
    prev_block_start = (target_block_index - 1) * block_size
    
    # 将 "admin=false" 改为 "admin=true;"
    original = b"admin=false"
    desired = b"admin=true;"
    
    print("\n执行位翻转攻击:")
    for i in range(len(original)):
        flip_pos = prev_block_start + i
        flip_value = original[i] ^ desired[i]
        modified_ciphertext[flip_pos] ^= flip_value
        print(f"位置 {flip_pos}: '{chr(original[i])}' -> '{chr(desired[i])}' (翻转: 0x{flip_value:02x})")
    
    # 解密验证
    result = decrypt_no_check(bytes(modified_ciphertext))
    print(f"\n解密结果: {result}")
    
    # 清理显示
    clean_result = b""
    for byte in result:
        if 32 <= byte <= 126:  # 可打印ASCII字符
            clean_result += bytes([byte])
        else:
            clean_result += b"."
    
    print(f"可读内容: {clean_result}")
    
    if b"admin=true" in result:
        print("🎉 CBC位翻转攻击成功！")
        return True
    else:
        print("❌ 攻击失败")
        return False

def guaranteed_success_attack():
    """
    保证成功的CBC位翻转攻击
    """
    key = os.urandom(16)
    iv = os.urandom(16)
    
    def encrypt(data):
        cipher = AES.new(key, AES.MODE_CBC, iv)
        return cipher.encrypt(pad(data, 16))
    
    def decrypt_no_check(ciphertext):
        cipher = AES.new(key, AES.MODE_CBC, iv)
        return cipher.decrypt(ciphertext)
    
    # 方法：使用两个完整块，第二个块包含目标数据
    block1 = b"A" * 16  # 第一个块：任意数据
    block2 = b"admin=0;data=end"  # 第二个块：目标数据，正好16字节
    
    original = block1 + block2
    print(f"\n保证成功演示 - 原始数据:")
    print(f"块1: {block1}")
    print(f"块2: {block2}")
    
    ciphertext = encrypt(original)
    
    # 攻击：修改第一个块来影响第二个块
    modified = bytearray(ciphertext)
    
    # 我们要将第二个块中的 "admin=0" 改为 "admin=1"
    # 第二个块的第一个字符受第一个块的第一个字符影响
    # 具体来说：block2[6]（'0'的位置）受 block1[6] 影响
    
    # 计算需要翻转的值
    # block2_decrypted[6] = decrypt(ciphertext[16+6]) ^ ciphertext[6]
    # 我们要让 block2_decrypted[6] 从 '0' 变成 '1'
    # 所以需要：ciphertext[6] = ciphertext[6] ^ '0' ^ '1'
    
    flip_value = ord('0') ^ ord('1')
    modified[6] ^= flip_value
    
    print(f"\n执行攻击:")
    print(f"修改位置 6: 翻转值 0x{flip_value:02x} ('0' -> '1')")
    
    result = decrypt_no_check(bytes(modified))
    
    print(f"\n攻击结果:")
    print(f"块1: {result[:16]}")
    print(f"块2: {result[16:32]}")
    
    if b"admin=1" in result[16:32]:
        print("保证成功的攻击演示完成！")
        return True
    return False

if __name__ == "__main__":
    print("=== 真正能工作的CBC位翻转攻击 ===\n")
    
    # 方法1：精确控制块边界
    success1 = working_cbc_bit_flip_attack()
    
    if not success1:
        print("\n" + "="*60)
        # 方法2：保证成功的方法
        success2 = guaranteed_success_attack()
    
    
