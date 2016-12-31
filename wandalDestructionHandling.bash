echo """Powielacz i generator obsługują jedną kolejkę,
czas pomiędzy kontrolami jest krótki,
aby powielacz naprawiał wszystko odpowiednio szybko
generator tworzy duzo odbiornikow, aby bylo sporo wandli w tym,
mozna zobaczyc jak wszystko jest korygowane"""

rm -rf results generatorLogs powielaczLogs .secret fifos

bash createFifos.bash 1

echo
echo "logi w  generatorLogs, powielaczLogs, results"

(./datownik.out -m0.5 | ./powielacz.out -c1 -pfifos/ -f1200 -dpowielaczLogs) &
(./generator.out -D0.1 -B23 -l10:20 -m0.5:1 -d0.2 -pfifos/ -c1 -Oresults -LgeneratorLogs)
