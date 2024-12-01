#!/bin/bash

# Очистка временных файлов после тестов
cleanup() {
  echo "Cleaning up..."
  rm -f result_s21_grep.txt result_grep.txt test1.txt test2.txt test3.txt test4.txt patterns.txt empty.txt test_large.txt test_large1.txt test_large2.txt
}

trap cleanup EXIT  # Оставляем файлы для анализа

# Функция для выполнения одного теста
run_test() {
  local description=$1
  local s21_grep_cmd=$2
  local grep_cmd=$3

  echo "Running test: $description"
  eval "$s21_grep_cmd" > result_s21_grep.txt
  eval "$grep_cmd" > result_grep.txt

  if diff -q result_s21_grep.txt result_grep.txt > /dev/null; then
    echo "Test passed"
  else
    echo "Test failed"
    echo "s21_grep output:"
    cat result_s21_grep.txt
    echo "grep output:"
    cat result_grep.txt
    exit 1
  fi
}

# Подготовка тестовых файлов
echo "Preparing test files"
cat > test1.txt << EOF
hello
world
Hello
HELLO
line1
line2
line3
EOF

cat > test2.txt << EOF
apple
banana
cherry
Apple
Banana
EOF

cat > test3.txt << EOF
line1 line2
line3
EOF

cat > test4.txt << EOF
12345
abcde
ABCDE
54321
EOF

cat > patterns.txt << EOF
hello
world
EOF

touch empty.txt

# Тесты

# Основные опции
run_test "Простой поиск (-e)" \
  "./s21_grep -e 'hello' test1.txt" \
  "grep -e 'hello' test1.txt"

run_test "Поиск с несколькими шаблонами (-e)" \
  "./s21_grep -e 'hello' -e 'world' test1.txt" \
  "grep -e 'hello' -e 'world' test1.txt"

run_test "Поиск без учета регистра (-i)" \
  "./s21_grep -i 'hello' test1.txt" \
  "grep -i 'hello' test1.txt"

run_test "Инвертированный поиск (-v)" \
  "./s21_grep -v 'hello' test1.txt" \
  "grep -v 'hello' test1.txt"

run_test "Подсчет совпадений (-c)" \
  "./s21_grep -c 'hello' test1.txt" \
  "grep -c 'hello' test1.txt"

run_test "Вывод только имен файлов с совпадениями (-l)" \
  "./s21_grep -l 'hello' test1.txt test2.txt" \
  "grep -l 'hello' test1.txt test2.txt"

run_test "Показ номеров строк (-n)" \
  "./s21_grep -n 'hello' test1.txt" \
  "grep -n 'hello' test1.txt"

run_test "Скрыть имена файлов (-h)" \
  "./s21_grep -h 'hello' test1.txt test2.txt" \
  "grep -h 'hello' test1.txt test2.txt"

run_test "Подавление ошибок (-s)" \
  "./s21_grep -s 'error' nonexistent.txt" \
  "grep -s 'error' nonexistent.txt"

run_test "Поиск с шаблонами из файла (-f)" \
  "./s21_grep -f patterns.txt test1.txt test2.txt" \
  "grep -f patterns.txt test1.txt test2.txt"

run_test "Вывод только совпадающих частей (-o)" \
  "./s21_grep -o 'line[0-9]' test3.txt" \
  "grep -o 'line[0-9]' test3.txt"

# Комбинированные опции
run_test "Комбинация: -e, -i, -v" \
  "./s21_grep -e 'hello' -e 'world' -i -v 'Apple' test1.txt test2.txt" \
  "grep -e 'hello' -e 'world' -i -v 'Apple' test1.txt test2.txt"

run_test "Комбинация: -c, -h, -o" \
  "./s21_grep -c -h -o 'line[0-9]' test3.txt" \
  "grep -c -h -o 'line[0-9]' test3.txt"

run_test "Комбинация: -l, -s с отсутствующим файлом" \
  "./s21_grep -l -s 'error' test1.txt nonexistent.txt" \
  "grep -l -s 'error' test1.txt nonexistent.txt"

# Тесты с пустыми файлами
run_test "Пустой файл" \
  "./s21_grep 'hello' empty.txt" \
  "grep 'hello' empty.txt"

run_test "Отсутствие совпадений в файле" \
  "./s21_grep 'xyz' test1.txt" \
  "grep 'xyz' test1.txt"

# Тесты на поиск символов
run_test "Поиск цифр" \
  "./s21_grep '[0-9]' test4.txt" \
  "grep '[0-9]' test4.txt"

run_test "Поиск букв (с учетом регистра)" \
  "./s21_grep '[a-z]' test4.txt" \
  "grep '[a-z]' test4.txt"

run_test "Поиск букв (без учета регистра)" \
  "./s21_grep -i '[a-z]' test4.txt" \
  "grep -i '[a-z]' test4.txt"

run_test "Поиск специальных символов" \
  "./s21_grep '[!@#\$%^&*]' test4.txt" \
  "grep '[!@#\$%^&*]' test4.txt"

run_test "Поиск с использованием чередования (|)" \
  "./s21_grep '123|abc' test4.txt" \
  "grep '123\|abc' test4.txt"


run_test "Поиск с многострочным шаблоном (-z)" \
  "./s21_grep -z 'line1\nline2' test3.txt" \
  "grep -z 'line1\nline2' test3.txt"

# Тесты на производительность
run_test "Тест на большом файле" \
  "head -c 1000000 </dev/urandom > test_large.txt && ./s21_grep 'pattern' test_large.txt" \
  "head -c 1000000 </dev/urandom > test_large.txt && grep 'pattern' test_large.txt"

run_test "Несколько больших файлов" \
  "head -c 1000000 </dev/urandom > test_large1.txt && head -c 1000000 </dev/urandom > test_large2.txt && ./s21_grep 'pattern' test_large1.txt test_large2.txt" \
  "head -c 1000000 </dev/urandom > test_large1.txt && head -c 1000000 </dev/urandom > test_large2.txt && grep 'pattern' test_large1.txt test_large2.txt"

echo "Все тесты успешно завершены!"

# Очистка после завершения тестов
cleanup