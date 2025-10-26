def apply_padding(data: bytes, block_size: int) -> bytes:
    pad_length = block_size - len(data) % block_size  # 计算需要填充的字节数
    return data + bytes([pad_length] * pad_length)  # 用填充字节值填充数据


def remove_padding(padded_data: bytes) -> bytes:
    pad_size = padded_data[-1]  # 获取最后一个字节的值，该值代表填充的长度
    content, padding_section = padded_data[:-pad_size], padded_data[-pad_size:]  # 分离出内容部分和填充部分
    # 验证填充部分是否符合预期，即每个填充字节都应该与填充长度相同
    if not all(byte == pad_size for byte in padding_section):
        raise ValueError("Invalid padding detected")  # 如果验证失败，抛出异常
    return content  # 返回去除填充后的原始内容


# 测试案例 1：测试正确填充和移除填充的情况
try:
    remove_padding(apply_padding(b"ICE ICE BABY", 16))
    print("Padding and unpadding successful")  # 成功情况下的输出
except ValueError:
    print("Padding validation failed")  # 填充验证失败的输出

# 测试案例 2：手动测试有效的填充数据，期望移除填充正常
try:
    remove_padding(b"ICE ICE BABY\x05\x05\x05\x05")
    print("Padding and unpadding successful")
except ValueError:
    print("Padding validation failed")

# 测试案例 3：测试无效的填充数据，期望验证失败
try:
    remove_padding(b"ICE ICE BABY\x01\x02\x03\x04")
    print("Padding and unpadding successful")
except ValueError:
    print("Padding validation failed")
