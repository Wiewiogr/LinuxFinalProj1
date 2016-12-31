echo """Aby pokazać, że po przepełnieniu się bufora tworzona jest nowa kolejka
ustawiłem czas wysłania danych przez datownik na bardzo krótki,
gdy powielacz uruchowi funkcje kontrolna kolejka jest naprawiana """

rm -rf results generatorLogs powielaczLogs .secret fifos

bash createFifos.bash 2

echo
echo "logi w  generatorLogs, powielaczLogs, results"

(./datownik.out -m0.00005 | ./powielacz.out -c2 -pfifos/ -f120 -dpowielaczLogs) &
(./generator.out -D0.1 -B2 -l10:20 -m0.5:1 -d0.2 -pfifos/ -c2 -Oresults -LgeneratorLogs)
