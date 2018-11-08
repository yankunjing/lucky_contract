//@abi table rversion i64
struct [[eosio::table, eosio::contract("lucky")]] rversion {
    uint64_t rule;
    uint16_t version = 0;

    uint64_t primary_key() const {
        return rule;
    }

    EOSLIB_SERIALIZE(
            rversion,
            (rule)
            (version)
    )
};

typedef eosio::multi_index< name("rversion"), rversion > rversion_table;
