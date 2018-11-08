
if [ "$1" = "real" ]
then
  url=https://rpc.eosys.io:443
  contract=dbetactivity
elif [ "$1" = "beta" ]
then
  url=https://api-kylin.eosasia.one
  contract=gameactivity
elif [ "$1" = "local" ]
then
  url=http://127.0.0.1:7777
  contract=lucky
elif [ "$1" = "remote" ]
then
  url=http://192.168.199.232:8880
  contract=lucky
else
  echo "need phase"
  exit 0
fi
alias cleos='sudo docker exec -it eosio /opt/eosio/bin/cleos --url $url'

cleos push action $contract cgift '[[{"no":0,"energy":0,"price":"0.0000 EOS","rows":[{"min":0,"max":9885,"bonus":"0.0001 EOS"},{"min":9886,"max":9985,"bonus":"0.0010 EOS"},{"min":9986,"max":9993,"bonus":"0.0100 EOS"},{"min":9994,"max":9997,"bonus":"0.1000 EOS"},{"min":9998,"max":9999,"bonus":"1.0000 EOS"},{"min":10000,"max":10000,"bonus":"10.0000 EOS"}]},{"no":1,"energy":100000,"price":"0.0000 EOS","rows":[{"min":0,"max":9885,"bonus":"0.0005 EOS"},{"min":9886,"max":9985,"bonus":"0.0050 EOS"},{"min":9986,"max":9993,"bonus":"0.0500 EOS"},{"min":9994,"max":9997,"bonus":"0.5000 EOS"},{"min":9998,"max":9999,"bonus":"5.0000 EOS"},{"min":10000,"max":10000,"bonus":"50.0000 EOS"}]},{"no":2,"energy":1000000,"price":"0.0000 EOS","rows":[{"min":0,"max":9885,"bonus":"0.0010 EOS"},{"min":9886,"max":9985,"bonus":"0.0100 EOS"},{"min":9986,"max":9993,"bonus":"0.1000 EOS"},{"min":9994,"max":9997,"bonus":"1.0000 EOS"},{"min":9998,"max":9999,"bonus":"10.0000 EOS"},{"min":10000,"max":10000,"bonus":"100.0000 EOS"}]},{"no":3,"energy":10000000,"price":"0.0100 EOS","rows":[{"min":0,"max":9885,"bonus":"0.0050 EOS"},{"min":9886,"max":9985,"bonus":"0.0500 EOS"},{"min":9986,"max":9993,"bonus":"0.5000 EOS"},{"min":9994,"max":9997,"bonus":"5.0000 EOS"},{"min":9998,"max":9999,"bonus":"50.0000 EOS"},{"min":10000,"max":10000,"bonus":"500.0000 EOS"}]},],1]' -p $contract
cleos push action $contract setcoo '["dbetonesicbo"]' -p $contract
