import math
import secrets
from typing import Tuple

# ------------------------------
# 核心算法：EGCD 和 模乘法逆元 invmod
# ------------------------------
def egcd(a: int, b: int) -> Tuple[int, int, int]:
    """扩展欧几里得算法：返回 (gcd, x, y) 满足 a*x + b*y = gcd(a, b)"""
    if a == 0:
        return b, 0, 1
    else:
        gcd, x, y = egcd(b % a, a)
        return gcd, y - (b // a) * x, x

def invmod(e: int, phi: int) -> int:
    """模乘法逆元：返回 d 使得 (e*d) mod phi = 1，若不存在则抛出异常"""
    gcd, x, _ = egcd(e, phi)
    if gcd != 1:
        raise ValueError(f"模 {phi} 下 {e} 没有乘法逆元（e 和 phi 不互质）")
    return x % phi  # 确保结果为正整数

# ------------------------------
# 质数工具：小质数判断 + 大质数生成（基于概率性测试）
# ------------------------------
def is_prime(n: int, k: int = 5) -> bool:
    """米勒-拉宾素性测试（概率性）：k 越大准确性越高（默认 k=5 足够日常使用）"""
    if n <= 1:
        return False
    elif n <= 3:
        return True
    elif n % 2 == 0:
        return False
    
    # 分解 n-1 = d*2^s
    d = n - 1
    s = 0
    while d % 2 == 0:
        d //= 2
        s += 1
    
    # 执行 k 轮测试
    for _ in range(k):
        a = secrets.randbelow(n - 3) + 2  # 随机选择 [2, n-2] 中的整数
        x = pow(a, d, n)
        if x == 1 or x == n - 1:
            continue
        for _ in range(s - 1):
            x = pow(x, 2, n)
            if x == n - 1:
                break
        else:
            return False
    return True

def generate_prime(bit_length: int = 1024) -> int:
    """生成指定比特长度的大质数（默认1024位，可调整为2048位增强安全性）"""
    while True:
        # 生成随机奇数（质数除了2都是奇数，排除偶数提高效率）
        candidate = secrets.randbits(bit_length)
        if candidate % 2 == 0:
            candidate += 1
        # 先做小质数筛选（快速排除明显合数）
        small_primes = [3, 5, 7, 11, 13, 17, 19, 23, 29, 31]
        if any(candidate % p == 0 for p in small_primes):
            continue
        # 米勒-拉宾测试确认质数
        if is_prime(candidate):
            return candidate

# ------------------------------
# RSA 密钥生成
# ------------------------------
def generate_rsa_keys(bit_length: int = 1024, e: int = 3) -> Tuple[Tuple[int, int], Tuple[int, int]]:
    """
    生成RSA密钥对
    :param bit_length: 质数比特长度（默认1024位）
    :param e: 公钥指数（固定为3，任务要求）
    :return: (公钥 (e, n), 私钥 (d, n))
    """
    # 生成两个不同的大质数 p 和 q
    while True:
        p = generate_prime(bit_length)
        q = generate_prime(bit_length)
        if p != q:
            break
    
    n = p * q  # RSA 模
    phi = (p - 1) * (q - 1)  # 欧拉函数 φ(n)
    
    # 确保 e 和 phi 互质（e=3时，只要phi不是3的倍数即可）
    while math.gcd(e, phi) != 1:
        # 若不互质，重新生成 q（效率更高）
        q = generate_prime(bit_length)
        phi = (p - 1) * (q - 1)
    
    d = invmod(e, phi)  # 计算私钥指数
    return (e, n), (d, n)

# ------------------------------
# RSA 加解密核心函数
# ------------------------------
def rsa_encrypt(m: int, public_key: Tuple[int, int]) -> int:
    """加密：c = m^e mod n"""
    e, n = public_key
    if m >= n:
        raise ValueError(f"明文 {m} 不能大于等于模 n {n}，请拆分或使用更长比特的密钥")
    return pow(m, e, n)  # Python pow支持三参数，高效计算大整数模幂

def rsa_decrypt(c: int, private_key: Tuple[int, int]) -> int:
    """解密：m = c^d mod n"""
    d, n = private_key
    return pow(c, d, n)

# ------------------------------
# 字符串与数字转换工具（任务要求的"cheesy"方案）
# ------------------------------
def string_to_int(s: str) -> int:
    """字符串转整数：先转十六进制，再加上0x前缀转为整数"""
    hex_str = s.encode('utf-8').hex()  # 字符串→UTF-8字节→十六进制字符串
    return int(f"0x{hex_str}", 16)  # 十六进制字符串→整数

def int_to_string(num: int) -> str:
    """整数转字符串：先转十六进制，再转回UTF-8字符串"""
    hex_str = hex(num)[2:]  # 整数→十六进制字符串（去掉0x前缀）
    # 处理奇数长度的十六进制（确保是字节的整数倍）
    if len(hex_str) % 2 != 0:
        hex_str = '0' + hex_str
    return bytes.fromhex(hex_str).decode('utf-8')  # 十六进制→字节→字符串

# ------------------------------
# 测试函数（验证全流程）
# ------------------------------
def test_rsa():
    print("=" * 60)
    print("RSA 算法测试（小质数 + 数字42）")
    print("=" * 60)
    
    # 修复：更换小质数组合，确保 e=3 与 phi 互质（gcd(3,40)=1）
    p_small = 5     # 小质数1
    q_small = 11    # 小质数2
    n_small = p_small * q_small
    phi_small = (p_small - 1) * (q_small - 1)  # (5-1)*(11-1) = 4*10 = 40
    e_small = 3
    d_small = invmod(e_small, phi_small)  # 此时存在逆元（d=27）
    public_key_small = (e_small, n_small)
    private_key_small = (d_small, n_small)
    
    print(f"小质数 p: {p_small}")
    print(f"小质数 q: {q_small}")
    print(f"n = p*q: {n_small}")
    print(f"phi = (p-1)*(q-1): {phi_small}")
    print(f"公钥 (e, n): {public_key_small}")
    print(f"私钥 (d, n): {private_key_small}")
    
    # 测试数字42加解密
    m_test = 42
    c_test = rsa_encrypt(m_test, public_key_small)
    m_decrypted = rsa_decrypt(c_test, private_key_small)
    print(f"\n原始数字: {m_test}")
    print(f"加密后密文: {c_test}")
    print(f"解密后数字: {m_decrypted}")
    print(f"小质数测试 {'成功' if m_test == m_decrypted else '失败'}！")
    
    print("\n" + "=" * 60)
    print("RSA 算法测试（大质数 + 字符串加密）")
    print("=" * 60)
    
    # 大质数测试（1024位质数，生产级强度）
    bit_length = 1024
    public_key_big, private_key_big = generate_rsa_keys(bit_length)
    e_big, n_big = public_key_big
    d_big, _ = private_key_big
    
    print(f"大质数生成的 n（前20位）: {str(n_big)[:20]}...")
    print(f"公钥 (e, n前20位): ({e_big}, {str(n_big)[:20]}...)")
    print(f"私钥 (d前20位, n前20位): ({str(d_big)[:20]}..., {str(n_big)[:20]}...)")
    
    # 测试字符串加解密
    s_original = "Hello RSA! 这是一个测试字符串：1234567890"
    print(f"\n原始字符串: {s_original}")
    
    # 字符串→整数→加密
    m_str = string_to_int(s_original)
    c_str = rsa_encrypt(m_str, public_key_big)
    print(f"字符串转整数（前20位）: {str(m_str)[:20]}...")
    print(f"加密后密文（前20位）: {str(c_str)[:20]}...")
    
    # 解密→整数→字符串
    m_str_decrypted = rsa_decrypt(c_str, private_key_big)
    s_decrypted = int_to_string(m_str_decrypted)
    print(f"解密后字符串: {s_decrypted}")
    print(f"大质数字符串测试 {'成功' if s_original == s_decrypted else '失败'}！")

if __name__ == "__main__":
    test_rsa()
