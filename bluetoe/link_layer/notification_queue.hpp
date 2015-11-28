#ifndef BLUETOE_LINK_LAYER_NOTIFICATION_QUEUE_HPP
#define BLUETOE_LINK_LAYER_NOTIFICATION_QUEUE_HPP

#include <cstdint>
#include <cstdlib>
#include <utility>
#include <cassert>
#include <algorithm>
#include <iterator>

namespace bluetoe {
namespace link_layer {

    /**
     * @brief class responsible to keep track of those characteristics that have outstanding
     *        notifications.
     *
     * All operations on the queue must be reentrent / atomic!
     *
     * @param Size Number of characteristics that have notifications and / or indications enabled
     *
     * For all function, index is an index into a list of all the characterstics with notifications / indications
     * enable. The queue is implemented by an array that contains
     */
    template < std::size_t Size >
    class notification_queue
    {
    public:
        notification_queue();

        /**
         * @brief queue the indexed characteristic for notification
         *
         * Once a characteristic is queued for notification, the function
         * dequeue_indication_or_confirmation() will return { notification, index }
         * on a future call.
         *
         * If the given characteristic was already queued for notification, the function
         * will not have any side effects.
         *
         * @pre index < Size
         */
        void queue_notification( std::size_t index );

        /**
         * @brief queue the indexed characteristic for indication
         *
         * Once a characteristic is queued for indication, the function
         * dequeue_indication_or_confirmation() will return { indication, index }
         * om a future call.
         *
         * If the given characteristic was already queued for indication, or
         * if a indication that was send to a client was not confirmed yet,
         * the function will not have any side effects.
         *
         * @pre index < Size
         */
        void queue_indication( std::size_t index );

        /**
         * @brief to be called, when a ATT Handle Value Confirmation was received.
         *
         * If not outstanding confirmation for the given index is registered, the
         * function has not side effect.
         */
        void indication_confirmed( std::size_t index );

        enum entry_type {
            empty,
            notification,
            indication
        };

        /**
         * @brief return a next notification or indication to be send.
         *
         * For a returned notification, the function will remove the returned entry.
         * For a returned indication, the function will change the entry to 'unconfirmed' and
         * will not return any indications until indication_confirmed() is called for the
         * returned index.
         */
        std::pair< entry_type, std::size_t > dequeue_indication_or_confirmation();

        /**
         * @brief removes all entries from the queue
         */
        void clear_indications_and_confirmations();
    private:
        int at( std::size_t index );
        void add( std::size_t index, int );
        void remove( std::size_t index, int );

        static constexpr std::size_t bits_per_characteristc = 2;

        enum char_bits {
            notification_bit = 0x01,
            indication_bit   = 0x02
        };

        std::size_t     next_;
        std::uint8_t    queue_[ ( Size * bits_per_characteristc + 7 ) / 8 ];
        std::size_t     outstanding_confirmation_;

    };

    // implementation
    template < std::size_t Size >
    notification_queue< Size >::notification_queue()
    {
        clear_indications_and_confirmations();
    }

    template < std::size_t Size >
    void notification_queue< Size >::queue_notification( std::size_t index )
    {
        assert( index < Size );
        add( index, notification_bit );
    }

    template < std::size_t Size >
    void notification_queue< Size >::queue_indication( std::size_t index )
    {
        assert( index < Size );
        add( index, indication_bit );
    }

    template < std::size_t Size >
    void notification_queue< Size >::indication_confirmed( std::size_t index )
    {
        if ( index < Size && outstanding_confirmation_ != Size && index == outstanding_confirmation_ )
        {
            outstanding_confirmation_ = Size;
        }
    }

    template < std::size_t Size >
    std::pair< typename notification_queue< Size >::entry_type, std::size_t > notification_queue< Size >::dequeue_indication_or_confirmation()
    {
        bool ignore_first = true;

        // loop over all entries in a circle
        for ( std::size_t i = next_; ignore_first || i != next_; i = ( i + 1 ) % Size )
        {
            ignore_first = false;
            auto entry = at( i );

            if ( entry & indication_bit && outstanding_confirmation_ == Size )
            {
                outstanding_confirmation_ = i;
                next_ = ( i + 1 ) % Size;
                remove( i, indication_bit );
                return { indication, i };
            }
            else if ( entry & notification_bit )
            {
                next_ = ( i + 1 ) % Size;
                remove( i, notification_bit );
                return { notification, i };
            }

        }

        return { empty, 0 };
    }

    template < std::size_t Size >
    void notification_queue< Size >::clear_indications_and_confirmations()
    {
        next_ = 0;
        outstanding_confirmation_ = Size;
        std::fill( std::begin( queue_ ), std::end( queue_ ), 0 );
    }

    template < std::size_t Size >
    int notification_queue< Size >::at( std::size_t index )
    {
        const auto bit_offset  = ( index * bits_per_characteristc ) % 8;
        const auto byte_offset = index * bits_per_characteristc / 8;
        assert( byte_offset < sizeof( queue_ ) / sizeof( queue_[ 0 ] ) );

        return ( queue_[ byte_offset ] >> bit_offset ) & 0x03;
    }

    template < std::size_t Size >
    void notification_queue< Size >::add( std::size_t index, int bits )
    {
        assert( bits & ( ( 1 << bits_per_characteristc ) -1 ) );
        const auto bit_offset  = ( index * bits_per_characteristc ) % 8;
        const auto byte_offset = index * bits_per_characteristc / 8;
        assert( byte_offset < sizeof( queue_ ) / sizeof( queue_[ 0 ] ) );

        queue_[ byte_offset ] |= bits << bit_offset;
    }

    template < std::size_t Size >
    void notification_queue< Size >::remove( std::size_t index, int bits )
    {
        assert( bits & ( ( 1 << bits_per_characteristc ) -1 ) );
        const auto bit_offset  = ( index * bits_per_characteristc ) % 8;
        const auto byte_offset = index * bits_per_characteristc / 8;
        assert( byte_offset < sizeof( queue_ ) / sizeof( queue_[ 0 ] ) );

        queue_[ byte_offset ] &= ~( bits << bit_offset );
    }


}
}

#endif // include guard
