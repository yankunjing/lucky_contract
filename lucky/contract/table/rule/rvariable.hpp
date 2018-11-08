// Variables are included in code to reduce cpu time.
// The data in 'rvariable' table is used only by the client.
// Note the consistency of the data in both.
const int kv_none = 0;
const int kv_max_gift_log_size = 5;

const int32_t checksum_mask = 2042423;

enum variable_type {
    vt_none = 0,
    vt_max_gift_log_size = 1
};

//@abi table rvariable i64
struct [[eosio::table, eosio::contract("lucky")]] rvariable {
    uint64_t key;
    uint32_t value;

    uint64_t primary_key() const {
        return key;
    }

    EOSLIB_SERIALIZE(
            rvariable,
            (key)
            (value)
    )
};

typedef eosio::multi_index< name("rvariable"), rvariable > rvariable_table;
