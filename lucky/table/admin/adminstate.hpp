//@abi table adminstate i64
struct [[eosio::table, eosio::contract("lucky")]] adminstate {
    uint64_t owner;
    uint8_t pause;
    name coo;

    uint64_t primary_key() const {
        return owner;
    }

    EOSLIB_SERIALIZE(
            adminstate,
            (owner)
            (pause)
            (coo)
    )
};

typedef eosio::multi_index< name("adminstate"), adminstate > adminstate_table;
