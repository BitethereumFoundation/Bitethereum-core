/*
 * Copyright (c) 2015 Cryptonomex, Inc., and contributors.
 *
 * The MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#pragma once

#include <graphene/chain/vesting_balance_object.hpp>

namespace graphene { namespace chain {

    struct by_owner_address;
    
    class airdrop_balance_object : public abstract_object<airdrop_balance_object>
    {
        public:
            static const uint8_t space_id = protocol_ids;
            static const uint8_t type_id  = airdrop_balance_object_type;
       
            address owner_address;
            asset balance;
        
    };
    
    using airdrop_balance_multi_index_type = multi_index_container<
        airdrop_balance_object,
        indexed_by<
            ordered_unique< tag<by_id>,
                member<object, object_id_type, &object::id>
            >,
            ordered_unique< tag<by_owner_address>,
                member<airdrop_balance_object, address, &airdrop_balance_object::owner_address>
            >
        >
    >;
    
    using airdrop_balance_index = generic_index<airdrop_balance_object, airdrop_balance_multi_index_type>;

    
} }

FC_REFLECT_DERIVED( graphene::chain::airdrop_balance_object, (graphene::db::object),
                   (owner_address)(balance) )

