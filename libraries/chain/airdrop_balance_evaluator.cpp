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
        
        FC_ASSERT(d.head_block_time() <= AIRDROP_END_TIME, "can not claim airdrop after 2018/3/21 24:00 GMT");
        
        if (op.address_type == address::AddressType::ETH) {
            
            address balance_owner = address::get_address(op.signature, op.address_type, true);
            
            std::cout << balance_owner.to_string(op.address_type) << std::endl;
            
            auto& index = d.get_index_type<airdrop_balance_index>().indices().get<by_owner_address>();
            auto itr = index.find(balance_owner.to_string(op.address_type));
            
            
            FC_ASSERT(itr != index.end(), "can not find airdrop shares");
            
            airdrop_object = &(*itr);
            
        }else if(op.address_type == address::AddressType::BTC) {
            
            address balance_owner = address::get_address(op.signature, op.address_type, true);
            std::cout << balance_owner.to_string(op.address_type) << std::endl;
            
            auto& index = d.get_index_type<airdrop_balance_index>().indices().get<by_owner_address>();
            auto itr = index.find(balance_owner.to_string(op.address_type));
            
            
            if(itr == index.end()){
                
                balance_owner = address::get_address(op.signature, op.address_type, false);
                std::cout << balance_owner.to_string(op.address_type) << std::endl;
                
                auto& index = d.get_index_type<airdrop_balance_index>().indices().get<by_owner_address>();
                auto itr = index.find(balance_owner.to_string(op.address_type));
                
                FC_ASSERT(itr != index.end(), "can not find airdrop shares");
                
                airdrop_object = &(*itr);
                
            }else{
                airdrop_object = &(*itr);
            }
            
        }

        
        return {};
    }
   
    
    void_result airdrop_balance_claim_evaluator::do_apply(const airdrop_balance_claim_operation& op)
    {
        database& d = db();

        d.remove(*airdrop_object);
       
        asset delta = airdrop_object->balance;
        d.adjust_balance(op.account_to_deposit, delta );
        
        const auto& dynamic_properties = d.get_dynamic_global_properties();
        d.modify(dynamic_properties, [&](dynamic_global_property_object& p) {
           p.total_airdrop_claim += delta.amount;
        });
        return {};
    }
   void_result airdrop_end_evaluator::do_evaluate(const airdrop_end_operation& op)
   {
      database& d = db();
      
       auto t = d.head_block_time();
      FC_ASSERT(t > AIRDROP_END_TIME, "can not end airdrop before 2018/3/21 24:00 GMT");
       
       auto end_time = d.get_dynamic_global_properties().airdrop_end_time;
       FC_ASSERT(end_time == time_point_sec(0), "can not end airdrop again");
       
      return {};
   }
   void_result airdrop_end_evaluator::do_apply(const airdrop_end_operation& op)
   {
      database& d = db();
      
      const asset_dynamic_data_object& core = asset_id_type(0)(d).dynamic_asset_data_id(d);
      const share_type total_airdrop_unclaim  =share_type(TOTAL_AIR_DROP)-d.get_dynamic_global_properties().total_airdrop_claim;
       
       
       if (total_airdrop_unclaim > share_type(TOTAL_MINING_AMOUNT)) {
           asset delta = asset(total_airdrop_unclaim - share_type(TOTAL_MINING_AMOUNT), asset_id_type());
           d.adjust_balance( GRAPHENE_DEV_ACCOUNT,  delta); // .TBD
           
           d.modify(core, [&]( asset_dynamic_data_object& _core ){
               _core.current_supply -= share_type(TOTAL_MINING_AMOUNT);
           });
           
       }else {
           
           asset delta = asset(total_airdrop_unclaim / 2 , asset_id_type());
           d.adjust_balance( GRAPHENE_DEV_ACCOUNT,  delta); // .TBD
           
           d.modify(core, [&]( asset_dynamic_data_object& _core ){
               _core.current_supply -= total_airdrop_unclaim / 2 ;
           });
       }
       
       
       const auto& dynamic_properties = d.get_dynamic_global_properties();
       d.modify(dynamic_properties, [&](dynamic_global_property_object& p) {
           p.airdrop_end_time = d.head_block_time();
       });

      return {};
   }
   
} } // namespace graphene::chain
