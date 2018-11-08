#pragma once

class variable_control : public control_base {
private:
    name self;
    rule_controller<rvariable, rvariable_table> rvariable_controller;

public:
    // constructor
    //-------------------------------------------------------------------------
    variable_control(name _self)
            : self(_self)
            , rvariable_controller(_self, _self) {
    }

    // internal apis
    //-------------------------------------------------------------------------

    // actions
    //-------------------------------------------------------------------------
    rule_controller<rvariable, rvariable_table>& get_rvariable_rule() {
        return rvariable_controller;
    }
};
