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
     
   address address::get_address(fc::ecc::compact_signature _signature, AddressType type){
       
       fc::sha256 sign_hash;
       bool bcheckCanonical = true;
       
       if(type == AddressType::ETH){
           
           std::string prefix = "\x19";
           prefix += "Ethereum Signed Message:\n16";
           prefix += AIRDROP_SIGN_STRING;
           
           // TBD
           std::cout << prefix << std::endl;
           
           sign_hash = fc::sha256((char*)dev::sha3(prefix).data(), 32);
           
           auto str = fc::to_hex(sign_hash.data(), sign_hash.data_size());
           std::cout << str << std::endl;
           
           bcheckCanonical = false;
       }
       else {
           sign_hash= fc::sha256::hash(AIRDROP_SIGN_STRING);
       }
       
       auto balance_public_key = fc::ecc::public_key::get_uncompress_public_key(_signature, sign_hash, bcheckCanonical);
      
       std::cout << fc::to_hex(balance_public_key.data, 65) << std::endl;
       return address(balance_public_key, type);
  }

   address::address( const fc::ecc::public_key& pub )
   {

      auto dat = pub.serialize();
      addr = fc::ripemd160::hash( fc::sha512::hash( dat.data, sizeof( dat ) ) );
   }

   address::address( const fc::ecc::public_key_point_data  pub, AddressType type){
      auto h =dev::sha3(dev::bytesConstRef((unsigned char *)pub.data+1,64));
      memcpy(addr.data(), (unsigned char *)h.data() + 12, 20);
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

   std::string address::to_string(bool base58){
      if(base58)
         return std::string(*this);
      else
         return fc::to_hex(addr.data(),addr.data_size() );
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
