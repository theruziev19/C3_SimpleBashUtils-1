#!/bin/bash

# Очистка временных файлов после тестов
cleanup() {
  echo "Cleaning up..."
  rm -f result_s21_cat.txt result_cat.txt file1.txt file2.txt empty.txt
}

trap cleanup EXIT

# Функция для выполнения одного теста
run_test() {
  local description=$1
  local s21_cat_cmd=$2
  local cat_cmd=$3

  echo "Running test: $description"
  eval "$s21_cat_cmd" > result_s21_cat.txt
  eval "$cat_cmd" > result_cat.txt

  if diff -q result_s21_cat.txt result_cat.txt > /dev/null; then
    echo "Test passed"
  else
    echo "Test failed"
    echo "s21_cat output:"
    cat result_s21_cat.txt
    echo "cat output:"
    cat result_cat.txt
    exit 1
  fi
}

# Подготовка тестового файла
echo "Preparing test files"
cat > file1.txt << EOF
Line 1 in file1
Line 2 in file1
EOF

cat > file2.txt << EOF
Line 1 in file2
Line 2 in file2
EOF

# Тесты
run_test "Simple file output" \
  "./s21_cat file1.txt" \
  "cat file1.txt"

run_test "Multiple files output" \
  "./s21_cat file1.txt file2.txt" \
  "cat file1.txt file2.txt"

run_test "File output with -n (number lines)" \
  "./s21_cat -n file1.txt" \
  "cat -n file1.txt"

run_test "File output with -b (number non-blank lines)" \
  "./s21_cat -b file1.txt" \
  "cat -b file1.txt"

run_test "File output with -s (squeeze blank lines)" \
  "./s21_cat -s file1.txt" \
  "cat -s file1.txt"

run_test "File output with -e (show $ at end of lines)" \
  "./s21_cat -e file1.txt" \
  "cat -e file1.txt"

run_test "File output with -t (show tabs as ^I)" \
  "./s21_cat -t file1.txt" \
  "cat -t file1.txt"

run_test "File output with -v (show non-printing characters)" \
  "./s21_cat -v file1.txt" \
  "cat -v file1.txt"

run_test "Multiple options combined: -n -s -b" \
  "./s21_cat -n -s -b file1.txt" \
  "cat -n -s -b file1.txt"

run_test "No match case (non-existent file)" \
  "./s21_cat nonexistent.txt" \
  "cat nonexistent.txt"

run_test "Empty file test" \
  "touch empty.txt && ./s21_cat empty.txt" \
  "cat empty.txt"

run_test "Multiple files with -e option" \
  "./s21_cat -e file1.txt file2.txt" \
  "cat -e file1.txt file2.txt"

echo "All tests passed successfully!"

# Очистка после завершения тестов
cleanup
