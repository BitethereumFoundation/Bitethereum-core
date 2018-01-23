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
#include <graphene/chain/protocol/types.hpp>
#include <graphene/chain/protocol/address.hpp>
#include <fc/crypto/elliptic.hpp>
#include <fc/crypto/base58.hpp>
#include <algorithm>
#include <fc/crypto/hex.hpp>
#include <eth/libdevcore/SHA3.h>
#include <btc/btc_key/base58.h>

namespace graphene {
  namespace chain {
   address::address(){}

   address::address( const std::string& str,ConstructFromStringType _t )
   {
      std::string prefix( GRAPHENE_ADDRESS_PREFIX );
      FC_ASSERT( is_valid( str, prefix ), "${str}", ("str",str) );

      std::vector<char> v ;
      if(_t=FromBase58){
         v= fc::from_base58(str.substr( prefix.size() ) );
         memcpy( (char*)addr._hash, v.data(), std::min<size_t>( v.size()-4, sizeof( addr ) ) );
      }
      else if(_t==FromHex)
      {
         if(str.substr(0,2)=="0x")
            fc::from_hex(str.substr(2,42 ),(char*)addr._hash, sizeof( addr ) );
         else
            fc::from_hex(str,(char*)addr._hash, sizeof( addr ) );
      }

   }

   bool address::is_valid( const std::string& base58str, const std::string& prefix )
   {
      const size_t prefix_len = prefix.size();
      if( base58str.size() <= prefix_len )
          return false;
      if( base58str.substr( 0, prefix_len ) != prefix )
          return false;
      std::vector<char> v;
      try
      {
		     v = fc::from_base58( base58str.substr( prefix_len ) );
      }
      catch( const fc::parse_error_exception& e )
      {
        return false;
      }

      if( v.size() != sizeof( fc::ripemd160 ) + 4 )
          return false;

      const fc::ripemd160 checksum = fc::ripemd160::hash( v.data(), v.size() - 4 );
      if( memcmp( v.data() + 20, (char*)checksum._hash, 4 ) != 0 )
          return false;

      return true;
   }
     
   address address::get_address(fc::ecc::compact_signature _signature, AddressType type, bool bUncompressPubKey){
       
       fc::sha256 sign_hash;
       bool bcheckCanonical = false;
       
       if(type == AddressType::ETH){
           // for ETH, v is at the end of signature
           fc::ecc::compact_signature new_signature;
           new_signature.data[0] = _signature.data[64];
           memcpy( (unsigned char*) new_signature.begin() + 1, (unsigned char*)_signature.begin(), _signature.size() - 1 );
           _signature = new_signature;
           
           std::string prefix = "\x19";
           prefix += "Ethereum Signed Message:\n16";
           prefix += AIRDROP_SIGN_STRING;
           
           sign_hash = fc::sha256((char*)dev::sha3(prefix).data(), 32);
           
           auto str = fc::to_hex(sign_hash.data(), sign_hash.data_size());

           auto pk = fc::ecc::public_key::get_uncompress_public_key(_signature, sign_hash, bcheckCanonical);
           
           return address(pk, type);
       }
       else if(type == AddressType::BTC){

           CHashWriter ss(0);
           ss << std::string("Bitcoin Signed Message:\n");
           ss << std::string(AIRDROP_SIGN_STRING);
           uint256 h = ss.GetHash();
           
           sign_hash = fc::sha256((const char*)h.begin(), h.size());
           
           
           if(bUncompressPubKey) {
               auto pk = fc::ecc::public_key::get_uncompress_public_key(_signature, sign_hash, bcheckCanonical);
               return address(pk, type);
           }
           
       }
       else {
           sign_hash= fc::sha256::hash(AIRDROP_SIGN_STRING);
       }
       
       fc::ecc::public_key pk = fc::ecc::public_key(_signature, sign_hash, bcheckCanonical);
      
       return address(pk, type);
  }

      
      address::address( const fc::ecc::public_key& pub,  AddressType type)
      {
          FC_ASSERT(AddressType::ETH != type, "no implementaion for AddressType::ETH");
          
          if ( AddressType::BTS == type ) {
              auto dat = pub.serialize();
              addr = fc::ripemd160::hash( fc::sha512::hash( dat.data, sizeof( dat ) ) );
              
          }else if ( AddressType::BTC == type ){
              auto dat = pub.serialize();
              addr = fc::ripemd160::hash( fc::sha256::hash( dat.data, sizeof( dat ) ) );

          }
          
      }

   address::address( const fc::ecc::public_key_point_data  pub, AddressType type){

       if ( AddressType::ETH == type) {
           
           auto h =dev::sha3(dev::bytesConstRef((unsigned char *)pub.data+1,64));
           memcpy(addr.data(), (unsigned char *)h.data() + 12, 20);
           
       } else if(AddressType::BTC == type ) {
           addr = fc::ripemd160::hash( fc::sha256::hash( pub.data, sizeof( pub.data ) ) );
        
       }
       
       
   }
   address::address( const pts_address& ptsaddr )
   {
       addr = fc::ripemd160::hash( (char*)&ptsaddr, sizeof( ptsaddr ) );
   }

   address::address( const fc::ecc::public_key_data& pub )
   {
       addr = fc::ripemd160::hash( fc::sha512::hash( pub.data, sizeof( pub ) ) );
   }

   address::address( const graphene::chain::public_key_type& pub )
   {
       addr = fc::ripemd160::hash( fc::sha512::hash( pub.key_data.data, sizeof( pub.key_data ) ) );
   }

   address::operator std::string()const
   {

       fc::array<char,24> bin_addr;
       memcpy( (char*)&bin_addr, (char*)&addr, sizeof( addr ) );
       
       auto checksum = fc::ripemd160::hash( (char*)&addr, sizeof( addr ) );
       memcpy( ((char*)&bin_addr)+20, (char*)&checksum._hash[0], 4 );
       
       return GRAPHENE_ADDRESS_PREFIX + fc::to_base58( bin_addr.data, sizeof( bin_addr ) );

   }

   std::string address::to_string(AddressType address_type){

       if (address_type == AddressType::ETH) {
           return fc::to_hex(addr.data(), addr.data_size() );
       }
       else if (address_type == AddressType::BTC) {
           
           // 1 + 24 + 4
           fc::array<char,25> bin_addr;
           memcpy( (char*)&bin_addr+1, (char*)&addr, sizeof( addr ) );
           
           auto h1 = fc::sha256::hash( (char*)&bin_addr, 21 );
           auto checksum = fc::sha256::hash(h1);
           
           memcpy( ((char*)&bin_addr)+21, (char*)&checksum._hash[0], 4 );
           
           return fc::to_base58( bin_addr.data, sizeof( bin_addr ) );
       }
       
       return std::string(*this);
   }
     
} } // namespace graphene::chain

namespace fc
{
    void to_variant( const graphene::chain::address& var,  variant& vo )
    {
        vo = std::string(var);
    }
    void from_variant( const variant& var,  graphene::chain::address& vo )
    {
        vo = graphene::chain::address( var.as_string() );
    }
}
