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

#include <btc/btc_key/base58.h>
//#include <btc/btc_key/utilstrencodings.h>

using namespace graphene::chain;




//0x975a157fdda72fd0a3c5c03a26e749339b3d7dc8
//0x5c57424ebf62220fdb41cbae8926fe038e3cf5a537e279688feabec7aa1538f16d593e4c2f007c7b09b025931bc5e635b63d1943af0716771db1428b12322b911c
void recoverEthAddress()
{
    // grab 0 or more whitespace-delimited PTS addresses from stdin
    
    std::string str="5c57424ebf62220fdb41cbae8926fe038e3cf5a537e279688feabec7aa1538f16d593e4c2f007c7b09b025931bc5e635b63d1943af0716771db1428b12322b911c";
    
    fc::ecc::compact_signature _signature;
    fc::from_hex(str, (char *)_signature.begin(), _signature.size());
    auto addr = address::get_address(_signature, address::AddressType::ETH, true);
    std::cout << addr.to_string(address::AddressType::ETH) << std::endl;
}





//addr: 1LnVj2JHDfxMpkiPGXz2dtC182kApgyEhu
//pub: 02703cf322509d053e9d9f203efba9425220f0d27dbb41173b84c9e9ecb33b550f
//sig: IKPvAm2tOELWR7YeLMoN50BHgQqu+EbZ4RuKuQO3RVk/Su1hyS9+SjM2nNEiP6c+asfJt6AiyI0nBth11dWicG4=
void recoverBtcAddress()
{
    std::string strSig = "HHzPQPyAWy+0RuRYOiWr5Drt6Ysj2Rw40uhDriDOWTYDJo0INJrG/S70LEggn/AxbW8s7kxNYkAOLBI4Db0Rdhg=";
    std::string sig = fc::base64_decode(strSig);
    
    fc::ecc::compact_signature _signature;
    memcpy(_signature.begin(), sig.c_str(), _signature.size());
    
    auto addr = address::get_address(_signature, address::AddressType::BTC, true);
    std::cout << addr.to_string(address::AddressType::BTC) << std::endl;
    
    
    addr = address::get_address(_signature, address::AddressType::BTC, false);
    std::cout << addr.to_string(address::AddressType::BTC) << std::endl;
    
    
//    std::cout << "str signature is "<<fc::to_hex((const char*)_signature.begin(), 65) << std::endl;
    
//    CHashWriter ss1(0);
//    ss1 << std::string("Bitcoin Signed Message:\n");
//    ss1 << std::string(AIRDROP_SIGN_STRING);
//    
//    std::cout<< "str hash is " << HexStr(ss1.GetHash()) <<std::endl;
    
//    
//    CHashWriter ss(0);
//    ss << std::string("Bitcoin Signed Message:\n");
//    ss << std::string(AIRDROP_SIGN_STRING);
//    uint256 h = ss.GetHash();
//    
//    fc::sha256 digest((const char*)h.begin(), h.size());
//    
//    
//    fc::ecc::public_key pk = fc::ecc::public_key::public_key(_signature, digest, false);
//    
//    std::cout << "1: "<< fc::to_hex(pk.serialize().data, pk.serialize().size()) << std::endl;
//    
//    address btcAddress = address(pk, address::AddressType::BTC);
//    
//    std::cout << btcAddress.to_string() << std::endl;
    

}



int main(int argc, char** argv)
{
//    recoverEthAddress();
    
    recoverBtcAddress();
   
//    std::string s;
//    while( std::cin >> s )
//    {
//      std::cout << std::string( address( pts_address( s ) ) ) << std::endl;
//    }

   return 0;
}
