#pragma once

class admin_control : public control_base {
private:
    name self;
    adminstate_table adminvalues;

public:
    /// @brief
    /// Constructor
    admin_control(name _self)
            : self(_self)
            , adminvalues(_self, _self.value) {
    }

    // internal apis
    //-------------------------------------------------------------------------
    bool is_paused() {
        if (adminvalues.cbegin() == adminvalues.cend()) {
            return false;
        }

        return adminvalues.cbegin()->pause == 1;
    }

    void entry_check(name name) {
        if (name == self) {
            return;
        }

        assert_true(!is_paused(), "game is paused");
    }

    name get_coo() {
        assert_true(adminvalues.cbegin() != adminvalues.cend(), "no admin values yet");
        return adminvalues.cbegin()->coo;
    }

    // actions
    //-------------------------------------------------------------------------
    /// @brief
    /// Set coo
    /// Coo can issue admin materials to market
    /// @param name
    /// coo account name
    void setcoo(name name) {
        require_auth(self);
        if (adminvalues.cbegin() == adminvalues.cend()) {
            adminvalues.emplace(self, [&](auto &target) {
                target.owner = self.value;
                target.pause = 0;
                target.coo = name;
            });
        } else {
            adminvalues.modify(adminvalues.cbegin(), self, [&](auto &target) {
                target.coo = name;
            });
        }
    }

    /// @brief
    /// Pause game
    /// User can not play this game until to pause valse is set to false.
    /// @param pause
    /// normal: 0, pause: 1
    void setpause(uint8_t pause) {
        require_auth(self);
        assert_true(pause >= 0 && pause <= 1, "invalid pause value");
        assert_true(adminvalues.cbegin() != adminvalues.cend(), "no admin values");

        adminvalues.modify(adminvalues.cbegin(), self, [&](auto &target) {
            target.pause = pause;
        });
    }
};
