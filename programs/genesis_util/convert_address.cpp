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

/**
 * Convert BTC / PTS addresses to a Graphene address.
 */

#include <graphene/chain/pts_address.hpp>
#include <graphene/chain/protocol/address.hpp>
#include <fc/crypto/sha256.hpp>
#include <fc/crypto/hex.hpp>

#include <iostream>
#include <string>

using namespace graphene::chain;

int main(int argc, char** argv)
{
   // grab 0 or more whitespace-delimited PTS addresses from stdin
   
   std::string str="1c5c57424ebf62220fdb41cbae8926fe038e3cf5a537e279688feabec7aa1538f16d593e4c2f007c7b09b025931bc5e635b63d1943af0716771db1428b12322b91";
   fc::ecc::compact_signature _signature;
   fc::from_hex(str,(char *)_signature.begin(),_signature.size());
   auto addr=address::get_address(_signature,address::Address_type::ETH);
   std::cout<<addr.to_string(false)<<std::endl;
   
   std::string s;
   while( std::cin >> s )
   {
      std::cout << std::string( address( pts_address( s ) ) ) << std::endl;
   }

   return 0;
}
