enum random_for {
    r4_gift,
};

struct giftinfo {
    uint8_t no;
    uint32_t point;

    EOSLIB_SERIALIZE(
            giftinfo,
            (no)
            (point)
    )
};

// 27 bytes
//@abi table player i64
struct [[eosio::table, eosio::contract("lucky")]] player {
    uint64_t owner;  // 8
    name account;
    uint32_t total_energy = 0;
    uint32_t today_energy = 0;
    uint32_t gift_time = 0;
    std::vector<giftinfo> gift_info;

    // 主键
    uint64_t primary_key() const {
        return owner;
    }

    // 索引
    name get_account() const {
        return account;
    }

    EOSLIB_SERIALIZE(
            player,
            (owner)
            (account)
            (total_energy)
            (today_energy)
            (gift_time)
            (gift_info)
    )
};

typedef eosio::multi_index<name("player"), player> player_table;
