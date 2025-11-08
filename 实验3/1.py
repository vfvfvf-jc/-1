from math import gcd

def is_valid_e(e, phi, p_minus_1, q_minus_1):
    """
    检查e是否满足3个核心条件（确保未加密信息数目最小）：
    1. gcd(e, φ(n)) == 1 → e与欧拉函数互质（RSA基本要求）
    2. gcd(e-1, p-1) == 2 → 最小公约数为2
    3. gcd(e-1, q-1) == 2 → 最小公约数为2
    """
    return (gcd(e, phi) == 1 and 
            gcd(e - 1, p_minus_1) == 2 and 
            gcd(e - 1, q_minus_1) == 2)

def sum_valid_e_values(p, q):
    # 验证p和q是否为质数（此处省略质数校验，题目已说明p=1009、q=3643为质数）
    p_minus_1 = p - 1  # 1008
    q_minus_1 = q - 1  # 3642
    phi = p_minus_1 * q_minus_1  # φ(n) = 1008×3642 = 3671136
    e_sum = 0

    # e的范围：1<e<φ(n)，且e为奇数（因e需与φ(n)互质，φ(n)为偶数，故e必为奇数）
    for e in range(3, phi, 2):
        if is_valid_e(e, phi, p_minus_1, q_minus_1):
            e_sum += e

    return e_sum

# 输入参数（题目给定）
p = 1009
q = 3643

# 计算结果
result = sum_valid_e_values(p, q)
print("The sum of valid e values is:", result)
