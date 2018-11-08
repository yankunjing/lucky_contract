#pragma once

class gift_control : public control_base {
private:
    name self;

    player_control &player_controller;
    logger_control &logger_controller;
    rule_controller<rgift, rgift_table> gift_rule_controller;

public:
    /// @brief
    /// Constructor
    gift_control(name _self, player_control &_player_controller, logger_control &_logger_controller)
        : self(_self)
        , gift_rule_controller(_self, _self)
        , player_controller(_player_controller)
        , logger_controller(_logger_controller) {
    }

    /// @brief
    /// Returns a controller that can CRUD the rule.
    /// @return
    /// Rule controller for gift rule
    rule_controller<rgift, rgift_table>& get_gift_rule() {
        return gift_rule_controller;
    }

    /// @brief
    /// The user can draw gift reward through this action.
    /// Increase history is recorded in the giftlog table.
    /// @param from
    /// account name who wants to draw
    /// @param no
    /// gift no
    void draw(name from, uint8_t no) {
        require_auth(self);
        player_controller.check_blacklist(from);
        player_controller.require_action_count(1);

        // find the rule
        auto rule = gift_rule_controller.get_table().find(no);
        assert_true(rule != gift_rule_controller.get_table().cend(), "no gift no");

        // load the player
        auto &players = player_controller.get_players();
        auto player = players.find(from.value);
        if (player == players.end()) {
            player = players.emplace(self, [&](auto& target) {
                target.owner = from.value;
                target.account = from;
            });
        }
        assert_true(player != players.cend(), "could not find player");
        auto gift_time = player->gift_time;
        auto gift_info = player->gift_info;
        auto today_energy = player->today_energy;

        auto current = time_util::getnow();
        if (gift_time < current) {
            today_energy = 0;
            gift_time = time_util::nextday();
            gift_info.clear();
        }

        for (auto iter = gift_info.begin(), end = gift_info.end(); iter != end; iter++) {
            assert_true(iter->no != no, "could not draw gift");
        }

        assert_true(today_energy >= rule->energy, "energy not enough");

        // 生成随机数
        auto rval = player_controller.begin_random(from, r4_gift, 0);
        int point = player_controller.random_range(rval, 10001);
        player_controller.end_random(from, rval, r4_gift, 0);

        // 设置标记位
        giftinfo v;
        v.no = no;
        v.point = point;
        gift_info.push_back(v);

        // 获取奖金数
        asset bonus;
        auto rows = rule->rows;
        for (auto iter = rows.begin(), end = rows.end(); iter != end; iter++) {
            if (point >= iter->min &&
                point <= iter->max) {
                bonus = iter->bonus;
                break;
            }
        }

        players.modify(player, self, [&](auto& target) {
            target.gift_info = gift_info;
            target.gift_time = gift_time;
        });

        std::string message = std::string("eosdicelucky:draw-gift:") +
                       std::to_string(no) + ":" +
                       std::to_string(point) + ":" +
                       from.to_string();
        send_transfer(from, bonus, message);

        gift_log glog;
        glog.dt = time_util::getnow();
        glog.no = no;
        glog.point = point;
        glog.energy = today_energy;
        glog.from = from;
        logger_controller.add_giftlog(glog, from);
    }

    /// @brief
    void send_transfer(name user, asset currency, const std::string &memo) {
        action(
        permission_level{self, name("active")},
        name("eosio.token"),
        name("transfer"),
        std::make_tuple(self, user, currency, memo)
        ).send();
    }
};
