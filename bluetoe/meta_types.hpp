#ifndef BLUETOE_META_TYPES_HPP
#define BLUETOE_META_TYPES_HPP

namespace bluetoe {

    namespace details {

        /*
         * A meta_type, that every option to a characteristic must have
         */
        struct valid_characteristic_option_meta_type {};

        /*
         * A meta_type that tags avery valid option to a service
         */
        struct valid_service_option_meta_type {};
    }
}

#endif
