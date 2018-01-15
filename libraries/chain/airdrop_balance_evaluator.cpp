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

namespace graphene { namespace chain {
    
    void_result airdrop_balance_claim_evaluator::do_evaluate(const airdrop_balance_claim_operation& op)
    {
        database& d = db();
        auto sign_hash=fc::sha256(AIRDROP_SIGN_STRING);
        fc::ecc::public_key balance_public_key=fc::ecc::public_key(op.signature,sign_hash);
        address balance_onwer=address(balance_public_key,true);
        auto& index = d.get_index_type<airdrop_balance_index>().indices().get<by_owner_address>();
        auto itr=index.find(balance_onwer);
        FC_ASSERT(itr!=index.end(),"can not find air drop shares");
        balance=itr->balance;
        return {};
    }
   
    
    void_result airdrop_balance_claim_evaluator::do_apply(const airdrop_balance_claim_operation& op)
    {
        database& d = db();

        d.adjust_balance(op.account_to_deposit, balance);
       
        //delete object
       
        return {};
    }
    
} } // namespace graphene::chain
