input_filename = 'caramel.pmd'
output_filename = 'caramel_pmd.h'
array_name = 'pmd_file'

# バイナリファイルの読み込み
with open(input_filename, 'rb') as f:
    data = f.read()

# C言語の配列コードを生成
c_code = []
c_code.append(f"// Generated from {input_filename}")
c_code.append(f"const unsigned char {array_name}[] = {{")

# 12バイトごとに改行を入れて整形
line_bytes = []
for i, byte in enumerate(data):
    line_bytes.append(f"0x{byte:02X}")
    if (i + 1) % 12 == 0:
        c_code.append("    " + ", ".join(line_bytes) + ",")
        line_bytes = []

# 残りのバイトを処理
if line_bytes:
    c_code.append("    " + ", ".join(line_bytes))

c_code.append("};")
c_code.append(f"const unsigned int {array_name}_len = {len(data)};\n")

# テキストファイルとして保存
with open(output_filename, 'w', encoding='utf-8') as f:
    f.write("\n".join(c_code))

print(f"C配列コードを {output_filename} に出力しました。")
