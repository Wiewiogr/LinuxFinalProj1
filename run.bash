rm -rf results generatorLogs powielaczLogs .secret fifos

bash createFifos.bash 8

echo "logi w  generatorLogs, powielaczLogs, results"

(./datownik.out -m0.5 | ./powielacz.out -c8 -pfifos/ -f120 -dpowielaczLogs) &
(./generator.out -D0.1 -B13 -l10:20 -m0.5:1 -d0.2 -pfifos/ -c8 -Oresults -LgeneratorLogs)
