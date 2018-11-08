#include <map>
#include <vector>
#include <string>
#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/transaction.hpp>
#include <eosiolib/crypto.h>
#include <cmath>

#define MAINTENANCE 0

#define N(expr) []() { constexpr auto x = #expr##_n; return x; }()

using eosio::key256;
using eosio::indexed_by;
using eosio::const_mem_fun;
using eosio::asset;
using eosio::print;
using eosio::permission_level;
using eosio::action;
using eosio::name;
using eosio::symbol;
using eosio::datastream;
using eosio::contract;

#include "table/rule/rgift.hpp"
#include "table/rule/rvariable.hpp"

#include "table/user/player.hpp"
#include "table/user/logger.hpp"
#include "table/outchain/transfer_action.hpp"
#include "table/outchain/random_val.hpp"
#include "table/admin/giftlog.hpp"
#include "table/admin/rversion.hpp"
#include "table/admin/adminstate.hpp"
#include "util/time_util.hpp"
#include "contract/control_base.hpp"
#include "contract/admin_control.hpp"
#include "contract/rule_controller.hpp"
#include "contract/variable_control.hpp"
#include "contract/player_control.hpp"
#include "contract/logger_control.hpp"
#include "contract/gift_control.hpp"

CONTRACT lucky : public contract {
private:
    // controls
    admin_control admin_controller;
    variable_control variable_controller;
    player_control player_controller;
    logger_control logger_controller;
    gift_control gift_controller;

    const char* ta_draw_gift = "draw_gift";

public:
    using contract::contract;

    lucky (name receiver, name code, datastream<const char*> ds)
        : contract(receiver, code, ds)
        , admin_controller(receiver)
        , variable_controller(receiver)
        , logger_controller(receiver)
        , player_controller(receiver, admin_controller, variable_controller)
        , gift_controller(receiver, player_controller, logger_controller) {
    }

    ACTION notify(name user, std::string msg) {
        require_auth(get_self());
        require_recipient(user);
    }

    // player related actions
    //-------------------------------------------------------------------------
    /// @abi action
    ACTION addenergy(name from, uint32_t energy) {
        player_controller.addenergy(from, energy);
    }

    /// @abi action
    ACTION drawgift(name from, uint8_t no) {
        gift_controller.draw(from, no);
    }

    ACTION draw(name from, uint8_t no) {
        require_auth(from);
        player_controller.check_blacklist(from);

        eosio::transaction txn{};
        const uint128_t sender_id = from.value << 32 | no;
        txn.actions.emplace_back(
            action(permission_level{get_self(), name("active")},
            get_self(),
            name("drawgift"),
            std::make_tuple(from, no)));
        txn.delay_sec = 1;
        txn.send(sender_id, from);
    }

    // rule ralated actions
    //-------------------------------------------------------------------------
    /// @abi action
    ACTION cgift(const std::vector<rgift> &rules, bool truncate) {
        gift_controller.get_gift_rule().create_rules(rules, truncate);
    }

    /// @abi action
    ACTION cvariable(const std::vector<rvariable> &rules, bool truncate) {
        variable_controller.get_rvariable_rule().create_rules(rules, truncate);
    }

    /// @abi action
    ACTION trule(name table, uint16_t size) {
        if (table == name("gift")) {
            gift_controller.get_gift_rule().truncate_rules(size);
        } else if (table == name("variable")) {
            variable_controller.get_rvariable_rule().truncate_rules(size);
        } else {
            eosio_assert(false, "could not find table");
        }
    }

    // admin related actions
    //-------------------------------------------------------------------------
    /// @abi action
    ACTION setpause(uint8_t pid) {
        admin_controller.setpause(pid);
    }

    /// @abi action
    ACTION setcoo(name coo) {
        admin_controller.setcoo(coo);
    }

    // eosio.token recipient
    // memo description spec
    // draw:{no}
    //-------------------------------------------------------------------------
    ACTION transfer(name sender, name receiver, asset quantity, std::string memo) {
        /*
        player_controller.eosiotoken_transfer(sender, receiver, [&](const auto &res) {
            if (res.action.size() == 0) {
                return;
            }

            if (res.action == ta_draw_gift) {
                gift_controller.draw(res.from, atoi(res.param.c_str()));
            } else {
                eosio_assert(false, "invalid transfer");
            }
        });
        */
    }
};

#define EOSIO_ABI( TYPE, MEMBERS ) \
extern "C" { \
   void apply( uint64_t receiver, uint64_t code, uint64_t action ) { \
      if (name(code) == name("eosio.token") && name(action) == name("transfer")) { \
         switch( action ) { \
            EOSIO_DISPATCH_HELPER( TYPE, (transfer) ) \
         } \
         /* does not allow destructor of thiscontract to run: eosio_exit(0); */ \
      } else \
      if( code == receiver ) { \
         switch( action ) { \
            EOSIO_DISPATCH_HELPER( TYPE, MEMBERS ) \
         } \
         /* does not allow destructor of thiscontract to run: eosio_exit(0); */ \
      } \
   } \
} \

EOSIO_ABI(lucky, (addenergy) (draw) (drawgift) (cgift) (trule) (setpause) (setcoo) ) // (clrall)
