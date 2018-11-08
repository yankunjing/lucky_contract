//@abi table giftlog i64
struct [[eosio::table, eosio::contract("lucky")]] giftlog {
    uint64_t no = 0;
    uint32_t dt = 0;
    uint16_t energy = 0;
    uint16_t flag = 0;
    uint32_t time = 0;
    std::string memo;

    uint64_t primary_key() const {
        return no;
    }

    EOSLIB_SERIALIZE(
            giftlog,
            (no)
            (dt)
            (energy)
            (flag)
            (time)
            (memo)
    )
};

typedef eosio::multi_index< name("giftlog"), giftlog > giftlog_table;
