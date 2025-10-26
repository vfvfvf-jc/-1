from Crypto.Cipher import AES
from Crypto.Util.Padding import pad, unpad
import os

def guaranteed_ecb_attack():
    """
    绝对保证成功的ECB攻击
    """
    key = os.urandom(16)
    
    def oracle(data):
        return AES.new(key, AES.MODE_ECB).encrypt(pad(data, 16))
    
    def decrypt(ct):
        return unpad(AES.new(key, AES.MODE_ECB).decrypt(ct), 16)
    
    
    # 方法：创建两个不同的密文，然后组合它们
    
    # 第一次：创建以"admin"结尾的块
    # 我们需要 "userdata=" + 输入 正好是完整块
    # "userdata=" 是9字节，需要7字节 -> 16字节
    input1 = b"A" * 7 + b"admin" + b"\x0b" * 11
    ct1 = oracle(b"userdata=" + input1 + b"&uid=100&role=user")
    blocks1 = [ct1[i:i+16] for i in range(0, len(ct1), 16)]
    
    # 第二次：创建正常数据，但要控制块结构
    # 让 "role=user" 在块边界
    input2 = b"B" * 9  # 计算好的长度
    ct2 = oracle(b"userdata=" + input2 + b"&uid=100&role=user") 
    blocks2 = [ct2[i:i+16] for i in range(0, len(ct2), 16)]
    
    # 攻击：组合两个密文
    # 取第二个密文的前两个块 + 第一个密文的第二个块
    attack_ct = blocks2[0] + blocks2[1] + blocks1[1]
    
    try:
        result = decrypt(attack_ct)
        print(f"解密结果: {result}")
        if b"role=admin" in result:
            print("攻击成功！获得admin权限")
            return True
    except Exception as e:
        print(f"解密错误: {e}")
    
    return False

# 运行保证成功的版本
if guaranteed_ecb_attack():
    print("问题6解决!")
