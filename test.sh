#!/usr/bin/env bash

# Exit immediately if any command below fails.
set -e

gcc -std=c11 -Wall -Werror -Wextra -pedantic -g3 -o file file.c


echo "Generating a test_files directory.."
mkdir -p test_files
rm -f test_files/*


echo "Generating test files.."
# ASCII TEXT TESTS (12+ tests)
printf "Hello, World!\n" > test_files/ascii.input
printf "Hello, World!" > test_files/ascii2.input
printf "The quick brown fox jumps over the lazy dog" > test_files/ascii3.input
printf "123 456 789\n" > test_files/ascii4.input
printf "Tab\there\n" > test_files/ascii5.input
printf "Line1\nLine2\nLine3\n" > test_files/ascii6.input
printf "Beep\x07sound\n" > test_files/ascii7.input
printf "Escape\x1Bsequence\n" > test_files/ascii8.input
printf "Back\x08space\n" > test_files/ascii9.input
printf "Carriage\rReturn\n" > test_files/ascii10.input
printf "Form\x0Cfeed\n" > test_files/ascii11.input
printf "No newline at end" > test_files/ascii12.input
printf "   \t\n  spaces and tabs  \n" > test_files/ascii13.input

# ISO-8859-1 TESTS (12+ tests)
printf "Hej, jeg hedder Søren\n" > test_files/iso1.input
printf "æøå ÆØÅ\n" > test_files/iso2.input
printf "København er dejlig\n" > test_files/iso3.input
printf "café résumé naïve\n" > test_files/iso4.input
printf "ñ é ü ö ä\n" > test_files/iso5.input
printf "£ ¥ © ® °\n" > test_files/iso6.input
printf "\xA0\xA1\xA2\xA3\n" > test_files/iso7.input
printf "\xE0\xE1\xE2\xE3\n" > test_files/iso8.input
printf "\xF0\xF1\xF2\xF3\n" > test_files/iso9.input
printf "Hello æøå world\n" > test_files/iso10.input
printf "123 Søren 456\n" > test_files/iso11.input
printf "\xE6\xF8\xE5\n" > test_files/iso12.input

# UTF-8 UNICODE TEXT TESTS (12+ tests)
printf "Hello 世界\n" > test_files/utf8_1.input
printf "中文测试\n" > test_files/utf8_2.input
printf "🎉 emoji test 🚀\n" > test_files/utf8_3.input
printf "😀😃😄\n" > test_files/utf8_4.input
printf "Привет мир\n" > test_files/utf8_5.input
printf "Математика\n" > test_files/utf8_6.input
printf "مرحبا العالم\n" > test_files/utf8_7.input
printf "Γειά σου κόσμε\n" > test_files/utf8_8.input
printf "こんにちは世界\n" > test_files/utf8_9.input
printf "日本語テスト\n" > test_files/utf8_10.input
printf "Hello 世界 World\n" > test_files/utf8_11.input
printf "שלום עולם\n" > test_files/utf8_12.input

# DATA TESTS (12+ tests)
printf "Hello,\x00World!\n" > test_files/data.input
printf "\x00\x00\x00" > test_files/data2.input
printf "\x80\x81\x82\x83\n" > test_files/data3.input
printf "Text\x9Fmore\n" > test_files/data4.input
printf "\xC0\x00\n" > test_files/data5.input
printf "\xE0\x80\x00\n" > test_files/data6.input
printf "\xFF\xFE\xFD\xFC\n" > test_files/data7.input
printf "\x01\x02\x03\x04\x05\x06\n" > test_files/data8.input
printf "\xC2" > test_files/data9.input
printf "\xE0\xA0" > test_files/data10.input
printf "\x80hello\n" > test_files/data11.input
printf "\xDE\xAD\xBE\xEF\n" > test_files/data12.input

# EMPTY TEST
printf "" > test_files/empty.input


echo "Running the tests.."
exitcode=0
for f in test_files/*.input
do
  echo ">>> Testing ${f}.."
  file    ${f} | sed -e 's/ASCII text.*/ASCII text/' \
                         -e 's/UTF-8 Unicode text.*/UTF-8 Unicode text/' \
                         -e 's/ISO-8859 text.*/ISO-8859 text/' \
                         -e 's/writable, regular file, no read permission/cannot determine (Permission denied)/' \
                         > "${f}.expected"
  ./file  "${f}" > "${f}.actual"

  if ! diff -u "${f}.expected" "${f}.actual"
  then
    echo ">>> Failed :-("
    exitcode=1
  else
    echo ">>> Success :-)"
  fi
done
exit $exitcode