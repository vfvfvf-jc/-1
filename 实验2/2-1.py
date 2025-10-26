def pad(raw: bytes, block_size: int) -> bytes:
    padding = block_size - len(raw) % block_size
    return raw + bytes([padding] * padding)
