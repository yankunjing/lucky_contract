// 24 bytes
struct giftrow {
    uint16_t min;
    uint16_t max;
    asset bonus;

    giftrow()
        : bonus(0, symbol("EOS", 4)) {
    }

    EOSLIB_SERIALIZE(
            giftrow,
            (min)
            (max)
            (bonus)
    )
};

//@abi table rgift i64
struct [[eosio::table, eosio::contract("lucky")]] rgift {
    uint64_t no = 0;
    uint32_t energy = 0;
    asset price;
    std::vector<giftrow> rows;

    rgift()
        : price(0, symbol("EOS", 4)) {
    }

    uint64_t primary_key() const {
        return no;
    }

    EOSLIB_SERIALIZE(
            rgift,
            (no)
            (energy)
            (price)
            (rows)
    )
};

typedef eosio::multi_index< name("rgift"), rgift > rgift_table;
