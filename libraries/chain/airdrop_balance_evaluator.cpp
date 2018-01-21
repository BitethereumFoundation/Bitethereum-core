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
#include <graphene/chain/airdrop_balance_evaluator.hpp>
#include <eth/libdevcore/SHA3.h>
#include <ostream>

namespace graphene { namespace chain {
    
    void_result airdrop_balance_claim_evaluator::do_evaluate(const airdrop_balance_claim_operation& op)
    {
        database& d = db();
        address balance_owner = address::get_address(op.signature, address::AddressType::ETH);
        
        std::cout << balance_owner.to_string() << std::endl;
        
        auto& index = d.get_index_type<airdrop_balance_index>().indices().get<by_owner_address>();
        auto itr = index.find(balance_owner.to_string());
        
        FC_ASSERT(itr != index.end(), "can not find airdrop shares");
        
        airdrop_object = &(*itr);

        return {};
    }
   
    
    void_result airdrop_balance_claim_evaluator::do_apply(const airdrop_balance_claim_operation& op)
    {
        database& d = db();

        d.remove(*airdrop_object);
        
        d.adjust_balance(op.account_to_deposit, airdrop_object->balance);
       
        return {};
    }
    
} } // namespace graphene::chain
