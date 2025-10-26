from Crypto.Cipher import AES
from Crypto.Util import Padding

# 固定的 AES 密钥
encryption_key = b"\xc6\xfe\xe2/\x97r|/\xeaY\xc5C\xbfi\x99\x97"


# 加密函数
def encrypt_user_data(user_data: bytes) -> bytes:
    # 构造数据，去掉用户数据中的 `&` 和 `=`
    sanitized_data = user_data.replace(b"&", b"").replace(b"=", b"")
    formatted_data = b"userdata=" + sanitized_data + b"&uid=10&role=user"

    # 在数据前添加 16 字节的填充，并进行 ECB 模式加密
    padded_data = Padding.pad((b"\x00" * 16) + formatted_data, 16)
    cipher = AES.new(encryption_key, AES.MODE_ECB)
    return cipher.encrypt(padded_data)


# 解密函数
def decrypt_user_data(encrypted_data: bytes) -> dict:
    # 使用 ECB 模式解密
    cipher = AES.new(encryption_key, AES.MODE_ECB)
    decrypted_data = cipher.decrypt(encrypted_data)

    # 去掉前面的 16 字节填充，并解析解密后的键值对
    unpadded_data = Padding.unpad(decrypted_data, 16)[16:]
    return {
        (key_value := item.split(b"=", maxsplit=1))[0].decode(): key_value[1]
        for item in unpadded_data.split(b"&")
    }


# 判断用户是否为管理员
def is_admin_user(encrypted_data: bytes) -> bool:
    decrypted = decrypt_user_data(encrypted_data)
    print(decrypted)
    return decrypted.get("role") == b"admin"


# 构造用户数据，其中包含伪造的 "admin" 角色
user_data_to_inject = b"A" * 7 + b"admin" + b"\x0b" * 14

# 加密用户数据
encrypted_data = encrypt_user_data(user_data_to_inject)

# 篡改密文，使其中的一部分被替换
modified_encrypted_data = encrypted_data[:64] + encrypted_data[32:48]

# 检查是否成功成为管理员
assert is_admin_user(modified_encrypted_data)
