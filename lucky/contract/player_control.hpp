#pragma once

class player_control : public control_base {
private:
    name self;
    admin_control &admin_controller;
    variable_control &variable_controller;
    player_table players;

    struct st_transfer {
        capi_name   from;
        capi_name   to;
        asset       quantity;
        std::string memo;
    };

public:
    // constructor
    //-------------------------------------------------------------------------
    player_control(name _self,
                   admin_control &_admin_controller,
                   variable_control &_variable_controller)
        : self(_self)
        , players(_self, _self.value)
        , admin_controller(_admin_controller)
        , variable_controller(_variable_controller) {
    }

    // internal apis
    //-------------------------------------------------------------------------
    player_table& get_players() {
        return players;
    }

    bool is_null_player(player_table::const_iterator player) {
        return player == players.end();
    }

    void increase_energy(player_table::const_iterator player, uint32_t energy) {
        auto current = time_util::getnow();
        players.modify(player, self, [&](auto& target) {
            if (target.gift_time < current) {
                target.today_energy = 0;
                target.gift_time = time_util::nextday();
                target.gift_info.clear();
            }

            target.total_energy += energy;
            target.today_energy += energy;
        });
    }

    template<typename T>
    void eosiotoken_transfer(name sender, name receiver, T func) {
        auto transfer_data = eosio::unpack_action_data<st_transfer>();
        eosio_assert(transfer_data.quantity.symbol == symbol("EOS", 4), "only accepts EOS for deposits");
        eosio_assert(transfer_data.quantity.is_valid(), "Invalid token transfer");
        eosio_assert(transfer_data.quantity.amount > 0, "Quantity must be positive");

        auto from = name(transfer_data.from);
        auto to = name(transfer_data.to);

        if (from == self) {
            check_blacklist(to);

            // todo: stockholder could be stockholder. so separate withdraw and dividend by message
        } else if (to == self) {
            check_blacklist(from);

            auto from_player = players.find(transfer_data.from);
            if (from_player == players.end()) {
                // system account could transfer eos to contract
                // eg) unstake, sellram, etc
                if (is_system_account(from)) {
                    // todo: add to the revenue for these.
                }
            }

            // player's deposit action
            transfer_action res;
            size_t center = transfer_data.memo.find(':');
            res.from = from;
            res.action = transfer_data.memo.substr(0, center);
            res.param = transfer_data.memo.substr(center + 1);
            res.quantity = transfer_data.quantity;
            func(res);
        }
    }

    bool is_system_account(name from) {
        /*
        if (from == name("eosio.bpay") ||
            from == name("eosio.msig") ||
            from == name("eosio.names") ||
            from == name("eosio.ram") ||
            from == name("eosio.ramfee") ||
            from == name("eosio.saving") ||
            from == name("eosio.stake") ||
            from == name("eosio.token") ||
            from == name("eosio.vpay") ) {
            return true;
        }
        */
        return false;
    }

    uint64_t seed_identity(name from);
    random_val begin_random(name from, random_for r4, int type) {
        return random_val(from.value, r4);
    }
    void end_random(name from, const random_val &val, random_for r4, int type) {
    }
    uint32_t random_range(random_val val, uint32_t to) {
        auto mixd = tapos_block_prefix() * tapos_block_num() + val.seed + val.value - current_time();
        const char *mixedChar = reinterpret_cast<const char *>(&mixd);

        capi_checksum256 result;
        sha256((char *)mixedChar, sizeof(mixedChar), &result);

        uint64_t random_num = *(uint64_t*)(&result.hash[0])
                            + *(uint64_t*)(&result.hash[8])
                            + *(uint64_t*)(&result.hash[16])
                            + *(uint64_t*)(&result.hash[24]);
        return (uint32_t)(random_num % to + 1);
    }

    int test_checksum(uint64_t checksum) {
        int32_t k = (checksum_mask & 0xFFFF);
        int32_t num = tapos_block_num();

        int64_t v1 = (checksum >> 32);
        int32_t v2 = get_checksum_value((checksum >> 16) & 0xFFFF);
        int32_t v3 = get_checksum_value((checksum) & 0xFFFF);
        assert_true((v1 % k) == v3, "checksum failure");
        assert_true((num - v1) < 120, "too old checksum");
        return (int)(num + v1 + v2 + v3) % k;
    }

    void require_action_count(int count) {
        char buffer[512];
        int actual_size = read_transaction(buffer, 512);
        eosio::datastream<const char *> ds(buffer, actual_size);
        eosio::transaction tx;
        ds >> tx;
        eosio_assert((tx.actions.end() - tx.actions.begin()) == count, "wrong number of actions in transaction");
    }

    int32_t get_checksum_value(int32_t value) {
        uint64_t a = checksum_mask >> 16;
        uint64_t b = checksum_mask & 0xFFFF;
        uint64_t c = value;
        uint64_t d = 1;
        for (int index = 0; index < a; index++) {
            d *= c;
            d %= b;
        }

        return d;
    }

    void check_blacklist(name from) {
        /*
        if (from == name("gameplayer11") ||
            from == name("gameplayer12") ||
            from == name("gameplayer13") ||
            from == name("gameplayer14") ||
            from == name("gameplayer15"))
        ) {
            assert_true(false, "blacklist rejected");
        }
        */
    }

    void require_coo_auth() {
        auto coo = admin_controller.get_coo();
        if (has_auth(coo)) {
            return;
        }

        require_auth(coo);
    }

    // actions
    //-------------------------------------------------------------------------
    void addenergy(name from, uint32_t energy) {
        require_coo_auth();
        assert_true(energy > 0, "energy does not match");

        auto iter = players.find(from.value);
        if (iter == players.end()) {
            iter = players.emplace(self, [&](auto& target) {
                target.owner = from.value;
                target.account = from;
            });
        }

        increase_energy(iter, energy);
    }
};
