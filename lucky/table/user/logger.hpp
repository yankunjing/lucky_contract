enum contrat_type {
    ct_none = 0,
    ct_gift
};

// ?? bytes
struct gift_log {
    uint32_t dt;
    name from;
    uint8_t no;
    uint32_t point;
    uint32_t energy;
};

//@abi table logger i64
struct [[eosio::table, eosio::contract("lucky")]] logger {
    uint64_t owner;
    std::vector<gift_log> giftlogs;

    uint64_t primary_key() const {
        return owner;
    }

    EOSLIB_SERIALIZE(
            logger,
            (owner)
            (giftlogs)
    )
};

typedef eosio::multi_index< name("logger"), logger > logger_table;
