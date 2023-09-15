sum=0
count=0

for arg in "$@"; do
  sum=$((sum + arg))
  count=$((count + 1))
done

aver=$((sum / count)) 

echo "Количество аргументов: $count"
echo "Среднее арифметическое: $aver"