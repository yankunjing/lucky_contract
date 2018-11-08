if [ "$1" = "real" ]
then
  url=https://tt.dbet.one
  contract=dbetactivity
elif [ "$1" = "beta" ]
then
  url=https://mainnet.meet.one
  contract=gameactivity
elif [ "$1" = "remote" ]
then
  url=http://192.168.199.232:8880
  contract=lucky
elif [ "$1" = "local" ]
then
  url=http://127.0.0.1:8888
  contract=lucky
else
  echo "need phase"
  exit 0
fi

sudo docker exec -it eosio /opt/eosio/bin/cleos --url $url set account permission lucky active '{"threshold": 1,"keys": [{"key": "EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV","weight": 1}], "accounts": [{"permission":{"actor":"lucky","permission":"eosio.code"},"weight":1}]}' -p lucky@owner

# cleos -u $url set contract $contract lucky -p $contract
sudo docker exec -it eosio /opt/eosio/bin/cleos --url $url set contract $contract /home/ykj/contracts/lucky/lucky -p $contract
