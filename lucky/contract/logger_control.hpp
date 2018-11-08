#pragma once

class logger_control : public control_base {
private:
    name self;
    logger_table logger;

public:
    logger_control(name _self)

        : self(_self)
        , logger(_self, _self.value) {
    }

    void add_giftlog(const gift_log& log, name from) {
        auto iter = logger.find(from.value);
        if (iter == logger.cend()) {
            logger.emplace(self, [&](auto& target) {
                target.owner = from.value;
                target.giftlogs.push_back(log);
            });
        } else {
            logger.modify(iter, self, [&](auto& target) {
                if (target.giftlogs.size() >= kv_max_gift_log_size) {
                    target.giftlogs.erase(target.giftlogs.begin());
                }
                target.giftlogs.push_back(log);
            });
        }
    }
};
