#!/bin/bash

# 定义要统计的目录
directories=("client" "common" "server")

# 定义要统计的文件扩展名
extensions=("cpp" "hpp")

# 初始化总行数
total_lines=0

# 显示文件信息
show_file_info() {
    local file=$1
    local lines=$2
    echo "$file: $lines lines"
}

# 统计函数（只返回行数）
count_lines() {
    local file=$1
    grep -c '^[[:space:]]*[^[:space:]]' "$file"
}

# 遍历目录和文件
for dir in "${directories[@]}"; do
    if [ -d "$dir" ]; then
        echo "===== $dir 目录 ====="
        dir_lines=0

        for ext in "${extensions[@]}"; do
            for file in $(find "$dir" -type f -name "*.$ext"); do
                lines=$(count_lines "$file")
                show_file_info "$file" "$lines"
                dir_lines=$((dir_lines + lines))
            done
        done

        echo "总计: $dir_lines 行"
        echo ""
        total_lines=$((total_lines + dir_lines))
    fi
done

echo "===== 总计 ====="
echo "非第三方库、非文档、非配置文件的非空行代码总数: $total_lines 行"
